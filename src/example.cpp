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

	Integer *res = nullptr;
	uint32_t jsz = 0;
	Integer zero(32, 0, PUBLIC);
	//op_sort(data->data, data->public_size, Bit(false));
	jsz = op_arr_merge_join(&res, zero, left, right, party);
	io->flush();

	cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
