#include "emp-sh2pc/emp-sh2pc.h"
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

	//cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;

	free(data);

}
