#include "emp-sh2pc/emp-sh2pc.h"
#include <utils/io.h>
#include <utils/oblisort.h>
#include <utils/memory.h>
#include <utils/operator.h>
#include <utils/params.h>
#include<thread>
#include <chrono>



using namespace emp;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);
	setup_semi_honest(io, party);

	std::vector<int32_t> attr = {1};
	std::vector<int32_t> vleft;
	std::vector<int32_t> vright;
	std::vector<int32_t> cacnt;
	if (party == ALICE){
		vleft = fnfetch_data(full_path + "left_0.txt");
		vright = fnfetch_data(full_path + "right_0.txt");
		cacnt = fnfetch_data(full_path + "cacnt_0");
	}
	else{
		vleft = fnfetch_data(full_path + "left_1.txt");
		vright = fnfetch_data(full_path + "right_1.txt");
		cacnt = fnfetch_data(full_path + "cacnt_1");
	}
	
	Data *left = op_recover(vleft, attr, 0, party);
	io->flush();

	Data *right = op_recover(vright, attr, 0, party);
	io->flush();

	Integer prev_cnt = op_recover(cacnt);
	io->flush();

	Integer *res = nullptr;
	uint32_t jsz = 0;
	jsz = op_arr_merge_join(&res, prev_cnt, left, right, party);
	io->flush();

	//cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;

	free(left);
	free(right);
	free(res);

}
