#include "emp-sh2pc/emp-sh2pc.h"
#include <utils/row.h>
#include <utils/io.h>
#include <utils/oblisort.h>
#include <utils/memory.h>
#include <utils/operator.h>
#include <utils/params.h>
#include<thread>
#include <chrono>

#define INT_SZ 32
#define TAB1_SZ 23
#define TAB2_SZ	20
#define TAB3_SZ	23
#define TAB4_SZ	20
#define TPCDS_MAXSTEPS 1829
#define CPDB_MAXSTEPS 920


using namespace emp;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;



// Configurations
int col_length0 = 64; 


void tcpds_q1_standard(Data *left, Data *right, uint32_t key_pos, uint32_t contr){
	auto t1 = high_resolution_clock::now();
	for (u_int32_t i=0; i < TPCDS_MAXSTEPS; i++){
		//std::cout << "Testing round -> "<< i <<std::endl;
		op_join(left, right, 0, 1);
		std::cout <<"Current round ->" <<i<<std::endl;
	}
	auto t2 = high_resolution_clock::now();
	/* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Total time for tpcds-q1:" << ms_double.count() << "ms" <<endl;
}



void test_millionare(int party, int number) {
	Integer a(32, number, ALICE);
	Integer b(32, number, BOB);
	Bit res = a > b;

	cout << "ALICE larger?\t"<< res.reveal<bool>()<<endl;
}

void test_sort(int party) {
	int size = 100;
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
	

	sort(res, size);
	for(int i = 0; i < 100; ++i)
		cout << res[i].reveal<int32_t>()<<endl;

	delete[] A;
	delete[] B;
	delete[] res;
}

int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);
	setup_semi_honest(io, party);

	std::vector<int32_t> attr = {1};
	std::vector<int32_t> vect;
	std::vector<int32_t> cacnt;
	if (party == ALICE){
		vect = fnfetch_data(full_path + "0.txt");
		cacnt = fnfetch_data(full_path + "cacnt_0");
	}
	else{
		vect = fnfetch_data(full_path + "1.txt");
		cacnt = fnfetch_data(full_path + "cacnt_1");
	}
	
	Data *data = op_recover(vect, attr, 0, party);
	io->flush();

	Integer prev_cnt = op_recover(cacnt);
	io->flush();

	op_filter_gt(data, prev_cnt, 5, party);
	io->flush();

#ifdef	DEBUG_JOIN
	std::vector<int32_t> vect;
	vect = fnfetch_data(full_path + "0.txt");
	std::vector<int32_t> vect2;
	vect2 = fnfetch_data(full_path + "1.txt");

	Data *data = op_recover(vect, attr, 0, party);
	io->flush();

	Data *data2 = op_recover(vect2, attr, 0, party);
	io->flush();

	//op_merg_join(data, data2, 0, 1);
	//op_sort(data2->data, data2->public_size, Bit(false));
	io->flush();
#endif

	//cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;

	free(data);

}
