#include "emp-sh2pc/emp-sh2pc.h"
#include <utils/row.h>
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


void synchron(Data *in, Integer prevcnt, int party){
	// use joint noise adding, this fixed noise is used for debug only
	int noise = -2;
	Integer noisy_cnt(32, 0, PUBLIC);
	Integer z(32, noise);
	Integer zero(32, 0);
	noisy_cnt = prevcnt + z;
	
	//compute fetch count
	uint32_t pos = in->fpos;
	uint32_t move = in->r_size;

	//truncate the fetch size to non-negative max(noisy_cnt, 0)
	Bit cond = (noisy_cnt > zero);
	noisy_cnt = If(cond, noisy_cnt, zero);
	uint32_t fetch_sz = noisy_cnt.reveal<int>(PUBLIC) * move;
	//std::cout<< "The fetch count for this round is"<< fetch_sz<<std::endl;

	op_sort(in->data, in->public_size, Bit(false));
	
#ifdef DEBUG2
	std::cout<< "Cache size is" << in->public_size << std::endl;
	for (uint32_t i=pos; i < in->public_size; i += move){
		std::cout<< in->data[i].reveal<int>(PUBLIC)<<std::endl;
	}
#endif

	/* Cache I/O operations*/
	// write to view
	Integer * out_arr = new Integer[fetch_sz];
	init_array(out_arr, fetch_sz, zero);
	if (fetch_sz < in->public_size)
		std::memcpy(out_arr, in->data, fetch_sz * sizeof(Integer));
	else
		std::memcpy(out_arr, in->data, in->public_size * sizeof(Integer));

	update_view(out_arr, fetch_sz, party);

	// update cache
	overwrite_cache(in, fetch_sz, party);

	// reset cardinality count
	reset_cacnt(party);

}


int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);
	setup_semi_honest(io, party);

	std::vector<int32_t> attr = {1};
	std::vector<int32_t> cache;
	std::vector<int32_t> cacnt;
	if (party == ALICE){
		cache = fnfetch_data(full_path + "cache_0");
		cacnt = fnfetch_data(full_path + "cacnt_0");
	}
	else{
		cache = fnfetch_data(full_path + "cache_1");
		cacnt = fnfetch_data(full_path + "cacnt_1");
	}
	
	Data *data = op_recover(cache, attr, 0, party);
	io->flush();

	Integer prev_cnt = op_recover(cacnt);
	io->flush();

	synchron(data, prev_cnt, party);
	io->flush();

	//cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;

	free(data);

}
