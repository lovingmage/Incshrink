#ifndef OPERATOR_H__
#define OPERATOR_H__

#include <utils/operator.h>
#include<thread>
#include <chrono>

using namespace emp;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


string obj_path = "/home/ypzhang0725/hermes/emp-sh2pc/data/";

// Define Data Structure
class Data {public:
    Integer * data;
    uint32_t public_size;
	uint32_t fpos;
	uint32_t r_size;
	std::vector<int32_t> attr;
    Integer real_size; // Reserved for Dubug
};


/* 
	Operator - oblivious sort cache (array of data)
		Inputs: The (cache) array to sort *arr; Array length N; Bit order;
		Outputs: sorted secure array *arr
*/
void op_sort(Integer *arr, uint32_t N, Bit dir)
{
	auto t1 = high_resolution_clock::now();
    arr_bitonic_sort(arr, 0, N, dir);
	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Sorting cost:" << ms_double.count() << "ms" <<endl;
}


/* 
	Operator - oblivious sort cache (array of data)
		Inputs: The (cache) array to sort *arr; Array length N; Bit order;
		Outputs: sorted secure array *arr
*/
void op_truncation_sort(Integer *arr, uint32_t N, Bit dir, uint32_t thread_num = 1)
{
	uint32_t subarr_length = N / thread_num;
	
	auto f = [](Integer *arr, uint32_t lo, uint32_t N, Bit dir) {
        arr_bitonic_sort(arr, lo, N, dir);
    };

	for (uint32_t i = 0; i < thread_num-1; i++){
		uint32_t shift = i*subarr_length;
		Integer *p = arr+shift;
		std::thread tsk(f, p, 0, subarr_length, Bit(false));
		tsk.join();
	}
	
}

/* Operator - recover cardinality count
*/
Integer op_recover(std::vector<int32_t> shares){
	Integer res(32, 0, PUBLIC);
	Integer a(32, shares[0], ALICE);
	Integer b(32, shares[0], BOB);
	res = a + b;
	std::cout << "Previous cadinality count is"<< res.reveal<int>(PUBLIC)<<std::endl;
	return res;
}
/*
	Operator - recover secret shared data
		Inputs: shares, attributes, isDummy pos, party
		Outputs: provisioned data structure 
*/
Data* op_recover(std::vector<int32_t> shares, std::vector<int32_t> attr, int32_t fpos, int party){
	int row_size = 0;
	Integer * res = new Integer[shares.size()];
	for (uint32_t i=0; i<attr.size(); ++i)
		row_size += attr[i];
	cout << "Fetched " << shares.size()/row_size << " rows." << endl;

	// Step 1.1 Specify Alice's input
	Integer * share0 = new Integer[shares.size()];
	for (uint32_t i=0; i < shares.size(); i++ )
		share0[i] = Integer(32, shares[i], ALICE);

	// Step 1.2 Specify Bob's input
	Integer * share1 = new Integer[shares.size()];
	for (uint32_t i=0; i < shares.size(); i++ )
		share1[i] = Integer(32, shares[i], BOB);

	// Step 1.3 Recover values
	auto t1 = high_resolution_clock::now();
	for (uint32_t i=0; i < shares.size(); i++ )
		res[i] = share0[i] + share1[i];
		//res[i] = share0[i] + share1[i];
	auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Recover cost:" << ms_double.count() << "ms" <<endl;

	Data * d = new Data;
	d->data = res;
	d->public_size = shares.size();
	d->attr = attr;
	d->fpos = fpos;
	d->r_size = row_size;
	d->real_size = Integer(32, d->public_size, PUBLIC);

	/*Get the size of materialized view instance */
	//std::cout << ( sizeof(Integer)* ( d->public_size + 1 ) + 2 * sizeof(uint32_t) + sizeof(d->attr) ) <<std::endl;
	//std::cout << ( sizeof(Integer)* ( d->public_size + 1 ) * (TAB1_SZ+TAB2_SZ) + 2 * sizeof(uint32_t) + sizeof(d->attr) ) <<std::endl;

	return d;
}

/* oblivious filter operator */
void op_filter_gt(Data *in, Integer prevcnt, uint32_t predicate, int party){
	Integer pred(32, predicate, PUBLIC);
	Integer piv(32, 0, PUBLIC);
	Integer * res = new Integer[in->public_size];

	Integer inc(32, 1, PUBLIC);
	Integer cacnt(32, 0, PUBLIC);
	//std::cout<<"Public size is:" << in->public_size <<std::endl;
	uint32_t pos = in->fpos;
	uint32_t move = in->r_size;
	for (uint32_t i=pos; i < in->public_size; i += move){
		Bit cond = (in->data[i] > pred);
		for (uint32_t j = 0; j < move; j++)
			res[i + j] = If(cond, in->data[i + j], piv);
			cacnt = If(cond, cacnt + inc, cacnt);
	}

	for (uint32_t i=0; i < in->public_size; i++)
		std::cout<<res[i].reveal<int>(BOB)<<std::endl;
	std::cout<<"True cardinality is:"<<cacnt.reveal<int>(BOB)<<std::endl;

	// write to cache
	// this is append only write
	for (uint32_t i=0; i < in->public_size; i++){
		int ss0 = res[i].reveal<int>(PUBLIC) + 10;
		int ss1 = -10;
			std::ofstream myfile;
			if (party == ALICE){
				std::string fpath = obj_path + "cache_0";
				myfile.open (fpath, std::ios_base::app);
				// hardcode - change to secret share
				myfile << ss0  << "\n";
				myfile.close();
			}
			else{
				std::string fpath = obj_path + "cache_1";	
				myfile.open (fpath, std::ios_base::app);
				myfile << ss1  << "\n";
				myfile.close();
			}
	}

	// write cardinality count to file
	// this is overwrite mode
	std::ofstream f_cnt;
	// hardcode randomness (test purpose only)
	int out_cnt_0 = cacnt.reveal<int>(PUBLIC) + 10 + prevcnt.reveal<int>(PUBLIC);
	int out_cnt_1 = -10;
	if (party == ALICE){
		std::string fpath = obj_path + "cacnt_0";
		f_cnt.open (fpath, std::ios::trunc);
		f_cnt << out_cnt_0  << "\n";
		f_cnt.close();
	}
	else{
		std::string fpath = obj_path + "cacnt_1";	
		f_cnt.open (fpath, std::ios::trunc);
		f_cnt << out_cnt_1  << "\n";
		f_cnt.close();
	}
}


/*
	Operator: count total # of non-dummy data
	Input: provisioned data structure Data*
	Outputs: count
*/
Integer op_cnt_ndummy(Data *in){
	Integer res(32, 0, PUBLIC);
	Integer piv(32, 0, PUBLIC);
	Integer inc(32, 1, PUBLIC);
	uint32_t pos = in->fpos;
	uint32_t move = in->r_size;
	for (uint32_t i=pos; i < in->public_size; i += move){
		Bit cond = (in->data[i] > piv);
		res = If(cond, res+inc, res);
	}
	return res;
}


/*
	Operator: compute joint table of two databases
	Input: Two provisioned data structure Data* left and Data *right
	Outputs: Join table for left x right
*/
void op_join(Data *left, Data *right, uint32_t key_pos, uint32_t contr){
	Integer zero(32, 0, PUBLIC);
	Integer one(32, 1, PUBLIC);
	uint32_t join_size = (left->public_size/left->r_size) * contr;
	uint32_t join_r_size = left->r_size + right->r_size - 1;
	Integer * res = new Integer[join_size * join_r_size];
	bool fg_only = (join_r_size == 1)? true : false;
	Integer * tmp = new Integer[right->public_size];
	auto t1 = high_resolution_clock::now();
	
	for (uint32_t i = 0; i < left->public_size; i+=left->r_size){
		if (i % 100 == 0){
				std::cout.precision(5);
				std::cout << "Current round " << i << std::endl;
		}
		for (uint32_t j = 0; j < right->public_size; j+=right->r_size){
			Bit condj = (left->data[i + key_pos] == right->data[j + key_pos]);	//join
			Bit condl = (left->data[i] != zero);								//non-dummy
			Bit condr = (right->data[j] != zero);								//non-dummy
			Bit cond = ( ( (condj == condl) == condr) == Bit(true));			//combined condition
			if (fg_only)
				tmp[j] = If(cond, one, zero);
			//TODO else
		}
		// truncate join output based on contr
		op_truncation_sort(tmp, (right->public_size), Bit(false), 3); 
		std::memcpy(res+i, tmp, contr*join_r_size*sizeof(Integer));
	}
	
	delete [] res;
	delete [] tmp;

	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Join cost:" << ms_double.count() << "ms" <<endl;

	return;
}


/*
	Operator: compute joint table of two databases
	Input: Two provisioned data structure Data* left and Data *right
	Outputs: Join table for left x right
*/
void op_merg_join(Data *left, Data *right, uint32_t key_pos, uint32_t contr){
	Integer zero(32, 0, PUBLIC);
	Integer one(32, 1, PUBLIC); 
	Integer* p0 = left->data;
	Integer* p1 = right->data;

	auto t1 = high_resolution_clock::now();

	// sort two join list
	op_sort(p0, left->public_size, Bit(false));
	op_sort(p1, right->public_size, Bit(false));

	uint32_t join_size = (left->public_size/left->r_size) * contr;
	uint32_t join_r_size = left->r_size + right->r_size - 1;
	Integer * res = new Integer[join_size * join_r_size]; 
	
	//merge join
	uint32_t begin = 0;
	for (uint32_t i = 0; i < left->public_size; i+=left->r_size ){
		bool cond = true;
		uint32_t j = begin;
		uint32_t budget = contr;
		while(cond){
			if (p0[i].reveal<int>() < p1[j].reveal<int>() || j >= right->public_size){
				cond = false;
				continue;
			}
			else if(p0[i].reveal<int>() > p1[j].reveal<int>()){
				j += right->r_size;
			}
			else{
				// TODO: write join to the res
				j += right->r_size;
				budget -= 1;
				if (budget <= 0)
					cond = false;
			}
		}
	}
	op_sort(p0, left->public_size, Bit(false));
	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Merge join cost:" << ms_double.count() << "ms" <<endl;

	return;
}

#endif
