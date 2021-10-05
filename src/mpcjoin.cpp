#include "emp-sh2pc/emp-sh2pc.h"
#include <cmath>
#include <chrono>
#define TPCDSDMAX 255
#define TPCDSTMAX 40000
#define CPDBDMAX 500
#define CPDBTMAX 2000
using namespace emp;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void test_millionare(int party, int number) {
	Integer a(32, number, ALICE);
	Integer b(32, number, BOB);
	Bit res = a > b;

	cout << "ALICE larger?\t"<< res.reveal<bool>()<<endl;
}

void test_sort(int party) {
	//int size = 2550;   //approximate #of data to sort for tpcds
    int size = 15000; //approximate #of data to sort for cpdb
	Integer *A = new Integer[size];
	Integer *B = new Integer[size];
	Integer *res = new Integer[size];

// First specify Alice's input
	for(int i = 0; i < size; ++i)
		A[i] = Integer(32, rand()%102400, ALICE);


// Now specify Bob's input
	for(int i = 0; i < size; ++i)
		B[i] = Integer(32, rand()%102400, BOB);

//Now compute
	for(int i = 0; i < size; ++i)
		res[i] = A[i] ^ B[i];
	
    auto t1 = high_resolution_clock::now();
	sort(res, size);
    auto t2 = high_resolution_clock::now();
    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms";

	//for(int i = 0; i < 100; ++i)
	//	cout << res[i].reveal<int32_t>()<<endl;

	delete[] A;
	delete[] B;
	delete[] res;
}

/* Pre-CMP function: Compte join table with window size
    TPC-ds: every time step call w times comp_join_one(), where w is window size.
            TPC-ds new sales data can not be joined with old return data.
            Only need to compute new_returns x new sales, new_returns x old_sales 
*/
void tpcds_precmp(int party, std::vector<int> vect, int vecSize){
    int size = vecSize;
    Integer *A = new Integer[size];
    Integer *B = new Integer[size];
    Integer *res = new Integer[size];
    Integer tmp(32, -1, PUBLIC);
    Integer ncnt(32, -1, PUBLIC);
    int cnt = 0;

    // Step 1. Specify Alice's input
	for(int i = 0; i < size; ++i)
		A[i] = Integer(32, vect[i], ALICE);

    // Step 1. Specify Bob's input
	for(int i = 0; i < size; ++i)
		B[i] = Integer(32, vect[i], BOB);

    // Step 1. Recover secret shared count
    ncnt = A[0] + B[0];

    auto t1 = high_resolution_clock::now();
    // Step 2 reveal results
	for(int i = 0; i < size; ++i){
        // effective bits DMAX
        if (i > TPCDSDMAX)
            break;
        for (int j= 0; j < size; ++j){
            tmp = (A[i] - B[j]);
            if(tmp.reveal<int32_t>() == 0){
                cnt +=1;
                break;
                //cout << "Join on record"<< i<<"-"<<j<<endl;
            }
        }
        //count << "random output join index"<<endl;
    }
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << "Elapse time: " << ms_int.count() << "ms\n";
    std::cout << "Elapse time: " << ms_double.count() << "ms\n";

    /* Generate secret shares */
    // The following cnt should be secret shared - This is for debug only.
    std::cout << "Total new entries: " << cnt + ncnt.reveal<int>() << endl;
} 

/* Join table with restricted contribution 

    @vect           input vector
        - vect[0]: secret share of n_cnt
        - vect[1:dbSize]: effective join keys, one key per record
        - vect[dbSize:]: paddings (zeros), reserved bits.

    @vecSize        sizeof(vect) 
    @dbSize         size of source db, specify the effective bits
    @contribution   one-time contribution upper bound
    
*/
void cpdb_precmp(int party, std::vector<int> vect, int vecSize, int contribution){
    int size = vecSize;
    Integer *A = new Integer[size];
    Integer *B = new Integer[size];
    Integer *res = new Integer[size];
    Integer tmp(32, -1, PUBLIC);
    Integer ncnt(32, -1, PUBLIC);
    int cnt = 0;
    int budget;

    // Step 1. Specify Alice's input
	for(int i = 0; i < size; ++i)
		A[i] = Integer(32, vect[i], ALICE);

    // Step 1. Specify Bob's input
	for(int i = 0; i < size; ++i)
		B[i] = Integer(32, vect[i], BOB);

    // Step 1. Recover secret shared count
    ncnt = A[0] + B[0];

    auto t1 = high_resolution_clock::now();
    // Step 2 reveal results
	for(int i = 0; i < size; ++i){
        budget = contribution;
        if(i > CPDBDMAX)
            break;
        for (int j= 0; j < size; ++j){
            tmp = (A[i] - B[j]);
            if(tmp.reveal<int32_t>() == 0){
                cnt +=1;
                budget-=1;
                //cout << "Join on record"<< i<<"-"<<j<<endl;
                if (budget <= 0)
                    break;
            }
        }
        // if (budget > 0)
            //count << "random output join index with size eq to budget"<<endl;
    }
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms";
    /* Generate secret shares */
    std::cout << "Total new entries: " << cnt + ncnt.reveal<int>() << endl;
} 

/* 
    Un-materialized join query  TPC-ds q1
    @vect       input vector
        - vect[0]: secret share of n_cnt
        - vect[1:dbSize]: effective join keys, one key per record
        - vect[dbSize:]: paddings (zeros), reserved bits.

    @vecSize    sizeof(vect) 
    @dbSize     size of source db, specify the effective bits

    Assign values to EMP array -> Recover secret shared n_cnt
*/
void join_query_q1(int party, std::vector<int> vect, int vecSize, int dbSize){
    int size = vecSize;
    Integer *A = new Integer[size];
    Integer *B = new Integer[size];
    Integer *res = new Integer[size];
    Integer tmp(32, -1, PUBLIC);
    Integer ncnt(32, -1, PUBLIC);
    int cnt = 0;

    // Step 1. Specify Alice's input
	for(int i = 0; i < size; ++i)
		A[i] = Integer(32, vect[i], ALICE);

    // Step 1. Specify Bob's input
	for(int i = 0; i < size; ++i)
		B[i] = Integer(32, vect[i], BOB);

    auto t1 = high_resolution_clock::now();
    // Step 2 reveal results
	for(int i = 0; i < size; ++i){
        if(i > dbSize)
            break;
        for (int j = i; j < i + TPCDSTMAX; ++j){
            tmp = (A[i] - B[j]);
            if(tmp.reveal<int32_t>() == 0){
                cnt +=1;
                //cout << "Reveal join - Index" << endl;
                break;
            }
        }
        // if (budget > 0)
            //count << "random output join index with size eq to budget"<<endl;
    }
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms";
    /* Generate secret shares */
    // The following cnt should be secret shared - This is for debug only.
    std::cout << "Total new entries: " << cnt + ncnt.reveal<int>() << endl;
} 

/* Un-materialized join query q2 */
void join_query_q2(int party, std::vector<int> vect, int vecSize, int dbSize, int contribution){
    int size = vecSize;
    Integer *A = new Integer[size];
    Integer *B = new Integer[size];
    Integer *res = new Integer[size];
    Integer tmp(32, -1, PUBLIC);
    Integer ncnt(32, -1, PUBLIC);
    int cnt = 0;
    int budget;

    // Step 1. Specify Alice's input
	for(int i = 0; i < size; ++i)
		A[i] = Integer(32, vect[i], ALICE);

    // Step 1. Specify Bob's input
	for(int i = 0; i < size; ++i)
		B[i] = Integer(32, vect[i], BOB);

    // Step 1. Recover secret shared count
    ncnt = A[0] + B[0];

    auto t1 = high_resolution_clock::now();
    // Step 2 reveal results
	for(int i = 0; i < size; ++i){
        budget = contribution;
        if(i > dbSize)
            break;
        for (int j = i; j < i + CPDBTMAX; ++j){
            tmp = (A[i] - B[j]);
            if(tmp.reveal<int32_t>() == 0){
                cnt +=1;
                budget-=1;
                //cout << "Join on record"<< i<<"-"<<j<<endl;
                if (budget <= 0)
                    break;
            }
        }
        // if (budget > 0)
            //count << "random output join index with size eq to budget"<<endl;
    }
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms";
    /* Generate secret shares */
    std::cout << "Total new entries: " << cnt + ncnt.reveal<int>() << endl;
} 

int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);
    string fileName;
	if(argc > 3)
		fileName = argv[3];
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    // read inputs from external file.
    fstream sharesFile;
    std::vector<int> vect;
	string fullPath = "/home/cw/mpc/emp-sh2pc/data/";
	
    sharesFile.open(fullPath + fileName, ios::in); //open a file to perform read operation using file object
    if (sharesFile.is_open()){   //checking whether the file is open
    string tp;
    while(getline(sharesFile, tp)){ //read data from file object and put it into string.
                        std::stringstream ss(tp);

                        for (int i; ss >> i;) {
                                vect.push_back(i);
                                if (ss.peek() == ',')
                                        ss.ignore();
                        }
                //cout << tp << "\n"; //print the data of the string
        }
        sharesFile.close(); //close the file object.
   }
	setup_semi_honest(io, party);
	//test_millionare(party, num);
    test_sort(party);                                     //test cache sort time
    //tpcds_precmp(party, vect, vect.size());
    //join_query_q1(party, vect, vect.size(), 255);
	//tpcds_precmp(party, vect, vect.size());
    //join_query_q2(party, vect, vect.size(), 450000, 10);
    cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
    
}
