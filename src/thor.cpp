#include "emp-sh2pc/emp-sh2pc.h"
#include <utils/row.h>
#include <utils/io.h>
#include <utils/oblisort.h>
#include <utils/memory.h>
#include <utils/operator.h>
#include<thread>
#include <chrono>

using namespace emp;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void synchron(Data *in, Integer prevcnt, int party){
	// use joint noise adding
	int noise = -2;
	Integer noisy_cnt(32, 0, PUBLIC);
	Integer z(32, noise);
	noisy_cnt = prevcnt + z;
	
	//compute fetch count
	uint32_t pos = in->fpos;
	uint32_t move = in->r_size;
	uint32_t fetch_sz = noisy_cnt.reveal<int>(PUBLIC) * move;
	std::cout<< "The fetch count for this round is"<< fetch_sz<<std::endl;

	op_sort(in->data, in->public_size, Bit(false));
	std::cout<< "Cache size is" << in->public_size << std::endl;
	//for (uint32_t i=pos; i < in->public_size; i += move){
	//	std::cout<< in->data[i].reveal<int>(PUBLIC)<<std::endl;
	//}

	// write to view
	// this is append only write
	for (uint32_t i=0; i < fetch_sz; i++){
		int ss0 = in->data[i].reveal<int>(PUBLIC) + 10;
		int ss1 = -10;
		std::ofstream myfile;
		if (party == ALICE){
				std::string fpath = obj_path + "view_0";
				myfile.open (fpath, std::ios_base::app);
				// hardcode - change to secret share
				myfile << ss0  << "\n";
				myfile.close();
		}
		else{
				std::string fpath = obj_path + "view_1";	
				myfile.open (fpath, std::ios_base::app);
				myfile << ss1  << "\n";
				myfile.close();
		}
	}

	// update cache
	// this is overwride write
	std::ofstream cachefile;
	int ss00 = in->data[fetch_sz].reveal<int>(PUBLIC) + 10;
	int ss10 = -10;
	if (party == ALICE){
				std::string fpath = obj_path + "cache_0";
				cachefile.open (fpath, std::ios::trunc);
				// hardcode - change to secret share
				cachefile << ss00  << "\n";
				cachefile.close();
		}
	else{
				std::string fpath = obj_path + "cache_1";	
				cachefile.open (fpath, std::ios::trunc);
				cachefile << ss10  << "\n";
				cachefile.close();
	}

	for (uint32_t i=fetch_sz+1; i < in->public_size; i++){
		std::cout << "Round is "<< i <<endl;
		int ss0 = in->data[i].reveal<int>(PUBLIC) + 10;
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
	int out_cnt_0 = 10 ;
	int out_cnt_1 = -10;
	if (party == ALICE){
		std::string fpath = obj_path + "cacnt_0";
		f_cnt.open (fpath, std::ios::trunc);
		// hardcode - change to secret share
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

// Configurations
string full_path = "/home/ypzhang0725/hermes/emp-sh2pc/data/";

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

	//cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;

	free(data);

}
