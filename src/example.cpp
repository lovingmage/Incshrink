#include "emp-sh2pc/emp-sh2pc.h"
#include <utils/row.h>
#include <utils/io.h>
#include <utils/oblisort.h>
#include <utils/memory.h>
#include <utils/operator.h>
#include <utils/params.h>


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
	std::vector<int32_t> vleft;
	std::vector<int32_t> vright;
	if (party == ALICE){
		vect = fnfetch_data(full_path + "a.txt");
		vleft = fnfetch_data(full_path + "/sample_db/join/a.txt");
		vright = fnfetch_data(full_path + "/sample_db/join/c.txt");
	}
	else{
		vect = fnfetch_data(full_path + "b.txt");
		vleft = fnfetch_data(full_path + "/sample_db/join/b.txt");
		vright = fnfetch_data(full_path + "/sample_db/join/d.txt");
	}
	
	Data *left = op_recover(vleft, attr, 0, party);
	io->flush();

	Data *right = op_recover(vright, attr, 0, party);
	io->flush();

	Integer res[10];
	//op_sort(data->data, data->public_size, Bit(false));
	op_arr_merge_join(res, left, right);
	io->flush();

	cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
