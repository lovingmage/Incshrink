#ifndef OPERATOR_H__
#define OPERATOR_H__

#include <utils/operator.h>
#include <utils/params.h>
#include <utils/io.h>
#include<thread>
#include <chrono>

#define RUNTIME_MAIN 0
#define VERBOSE_T 0

using namespace emp;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


/* 
	Operator - oblivious sort array (array of data)
		Inputs: The array to be sorted *arr; Array length N; Bit order;
		Outputs: sorted secure array *arr
*/
void op_sort(Integer *arr, uint32_t N, Bit dir)
{
#ifdef VERBOSE_T
	auto t1 = high_resolution_clock::now();
#endif

    arr_bitonic_sort(arr, 0, N, dir);

#ifdef VERBOSE_T
	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Sorting cost:" << ms_double.count() << "ms" <<endl;
#endif
	
}

/* 
	Operator - faster oblivious sort for growing array, this is achieved by define an upperbound 
				for the max possible cached data (2*blk_sz). And blk >= max appended tuples between 2 updates.

		Inputs: The partially sorted array *arr (sorted after position blk_sz);
				Array length N; 
				Threshold blk_sz; 
				Bit order;
		Outputs: sorted secure array *arr
*/
void op_fsort(Integer *arr, uint32_t N, uint32_t blk_sz, Bit dir)
{
#ifdef VERBOSE_T
	auto t1 = high_resolution_clock::now();
#endif	
	
	//if the total cache sz is smaller than upper bound 2*blk_sz
	//sort the entire cache
	if ( N < 2*blk_sz ){
		arr_bitonic_sort(arr, 0, N, dir);
	}
	//if the total cache sz is in (2*blk_sz, 4*blk_sz)
	//sort the first block (with size blk_sz) with !dir
	//merge the first two blocks (the 2nd blk is assumed to be sorted)
	else if ( N < 4*blk_sz ){
		arr_bitonic_sort(arr, 0, blk_sz, !dir);
		arr_bitonic_merge(arr, 0, 2*blk_sz, dir);
	}
	else{
		// currently treated same as ( N < 4*blk_sz )
		// TODO: 	(i)  	merge the first 2 blks, 
		//			(ii) 	flip the array side 
		//			(iii) 	merge the first 4 blks
		arr_bitonic_sort(arr, 0, blk_sz, !dir);
		arr_bitonic_merge(arr, 0, 2*blk_sz, dir);
	}

#ifdef VERBOSE_T
	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Sorting cost:" << ms_double.count() << "ms" <<endl;
#endif
}

/* 
	Operator - oblivious sort cache (the array of mem blocks)
		Inputs: The cache to be sorted *cache; cache size N; Cache blok size, rsz, Bit order;
		Outputs: sorted cache *cache
*/
void op_csort(Integer *cache, uint32_t N, uint32_t rsz, Bit dir)
{
	Integer arr[N/rsz];
	for (uint32_t i=0; i< N/rsz; i++)
		arr[i] = cache[i*rsz];

#ifdef VERBOSE_T
	auto t1 = high_resolution_clock::now();
#endif	

    cache_bitonic_sort(arr, cache, 0, rsz, N/rsz, dir);

#ifdef VERBOSE_T
	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Sorting cost:" << ms_double.count() << "ms" <<endl;
#endif
}

/* 
	Operator - oblivious array sort
		Inputs: The (cache) array to sort *arr; Array length N; Bit order;
		Outputs: sorted secure array *arr
*/
void op_truncation_sort(Integer *arr, uint32_t N, Bit dir, uint32_t thread_num = 2)
{
	uint32_t subarr_length = N / thread_num;
	
	auto f = [](Integer *arr, uint32_t lo, uint32_t N, Bit dir) {
        arr_bitonic_sort(arr, lo, N, dir);
    };

#ifdef VERBOSE_T
	auto t1 = high_resolution_clock::now();
#endif	

	std::thread tsk1(f, arr, 0, subarr_length - 1, Bit(true));
	std::thread tsk2(f, arr, subarr_length, N, Bit(false));

	tsk1.join();

	tsk2.join();

	arr_bitonic_merge(arr, 0, N, Bit(false));

#ifdef VERBOSE_T
	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Sorting cost:" << ms_double.count() << "ms" <<endl;
#endif
	/*
	for (uint32_t i = 0; i < thread_num; i++){
		uint32_t shift = i*subarr_length;
		Integer *p = arr+shift;
		std::thread tsk(f, p, 0, subarr_length, Bit(false));
		tsk.join();
	}
	*/
}

/* Operator - recover single secret shared value
*/
Integer op_recover(std::vector<int32_t> shares){
	Integer res(32, 0, PUBLIC);
	Integer a(32, shares[0], ALICE);
	Integer b(32, shares[0], BOB);
	res = a + b;
#ifdef DEBUG
	std::cout << "The recovered value is "<< res.reveal<int>(PUBLIC)<<std::endl;
#endif
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

#ifdef DEBUG
	cout << "Fetched " << shares.size()/row_size << " rows." << endl;
#endif

	// Step 1.1 Specify Alice's input
	Integer * share0 = new Integer[shares.size()];
	for (uint32_t i=0; i < shares.size(); i++ )
		share0[i] = Integer(32, shares[i], ALICE);

	// Step 1.2 Specify Bob's input
	Integer * share1 = new Integer[shares.size()];
	for (uint32_t i=0; i < shares.size(); i++ )
		share1[i] = Integer(32, shares[i], BOB);

	// Step 1.3 Recover values
#ifdef VERBOSE_T
	auto t1 = high_resolution_clock::now();
#endif	
	for (uint32_t i=0; i < shares.size(); i++ )
		res[i] = share0[i] + share1[i];

#ifdef VERBOSE_T
	auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Recover cost:" << ms_double.count() << "ms" <<endl;
#endif

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
#ifdef DEBUG
	for (uint32_t i=0; i < in->public_size; i++)
		std::cout<<res[i].reveal<int>(BOB)<<std::endl;
	std::cout<<"True cardinality is:"<<cacnt.reveal<int>(BOB)<<std::endl;
#endif
	// write to cache
	// this is append only write
	append_cache(res, in->public_size, party);

	// write cardinality count to file
	// this is overwrite mode
	update_cacnt(cacnt.reveal<int>(PUBLIC), prevcnt.reveal<int>(PUBLIC), party);
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
	Outputs: Join table for left x right; join size
*/

uint32_t op_arr_merge_join(Integer **res, Integer prevcnt, Data *left, Data *right, int party){
	Integer zero(32, 0, PUBLIC);
	Integer one(32, 1, PUBLIC); 
	op_sort(left->data, left->public_size, Bit(true));
	op_sort(right->data, right->public_size, Bit(true));
	uint32_t join_size = (left->public_size > right->public_size)? right->public_size : left->public_size;
	Integer *join = new Integer[join_size];
	init_array(join, join_size, zero);
#ifdef VERBOSE_T
	auto t1 = high_resolution_clock::now();
#endif
	uint32_t j=0;
	uint32_t k=0;
	for (uint32_t i=0; i < left->public_size; i++){	

		Bit cond_p = (left->data[i] <= zero);
		if(cond_p.reveal<bool>()){
			continue;
		}

		Bit cond_eq = (left->data[i] == right->data[j]);
		Bit cond_lt = (left->data[i] < right->data[j]);

		if (cond_eq.reveal<bool>()){
			join[k] = left->data[i];
			k+=1;
		}
		else if (cond_lt.reveal<bool>()){
			continue;
		}
		else{
			for (uint32_t t=j; t<right->public_size-1; t++){
				j+=1;
				Bit cond_leq = (left->data[i] <= right->data[j]);
				if(cond_leq.reveal<bool>()){
					Bit cond_eq2 = (left->data[i] == right->data[j]);
					if(cond_eq2.reveal<bool>()){
						join[k] = left->data[i];
						k+=1;
					}
					break;
				}
			}
		}
	}

#ifdef VERBOSE_T
	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Merge join cost:" << ms_double.count() << "ms" <<endl;
#endif

	*res = (Integer*)malloc(sizeof(Integer)*join_size);
	std::memcpy(*res, join, join_size * sizeof(Integer));

#ifdef RUNTIME_MAIN
	// write to cache
	// this is append only write
	append_cache(*res, join_size, party);

	// write cardinality count to file
	// this is overwrite mode
	update_cacnt(k, prevcnt.reveal<int>(PUBLIC), party);
#endif

#ifdef DEBUG
	for (uint32_t i=0; i< join_size; i++)
		std::cout<<join[i].reveal<int>()<<std::endl;
#endif
	
	return join_size;
}


#endif
