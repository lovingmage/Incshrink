#ifndef IO_H__
#define IO_H__

#include <utils/params.h>


using namespace emp;
using namespace std;

// I/O interface fetch data from file
std::vector<int32_t> fnfetch_data(string fn) {
	std::vector<int32_t> res;
	fstream sharesFile;			//view instance (secret shares)
	sharesFile.open(fn, ios::in);
	if (sharesFile.is_open()){   //checking whether the file is open
		string tp;
		while(getline(sharesFile, tp)){ //read data from file object and put it into string.
			std::stringstream ss(tp);
			for (int i; ss >> i;) {
				res.push_back(i);
				if (ss.peek() == ',')
					ss.ignore();
			}
		}
        sharesFile.close(); //close the file object.
	}
	return res;
}

void update_view(Integer *data, uint32_t sz, int party){
	for (uint32_t i=0; i < sz; i++){
		int ss0 = data[i].reveal<int>(PUBLIC) - RANDOM_SS;
		int ss1 = RANDOM_SS;
		std::ofstream myfile;
		if (party == ALICE){
				std::string fpath = obj_path + "view_0";
				myfile.open (fpath, std::ios_base::app);
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
}

void overwrite_cache(Data* in, uint32_t fetch_sz, int party){
	std::ofstream cachefile;
	int ss00 = in->data[fetch_sz].reveal<int>(PUBLIC) - RANDOM_SS;
	int ss10 = RANDOM_SS;
	if (party == ALICE){
				std::string fpath = obj_path + "cache_0";
				cachefile.open (fpath, std::ios::trunc);
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
		int ss0 = in->data[i].reveal<int>(PUBLIC) + 10;
		int ss1 = -10;
		std::ofstream myfile;
		if (party == ALICE){
				std::string fpath = obj_path + "cache_0";
				myfile.open (fpath, std::ios_base::app);
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
}


void reset_cacnt(int party){
	std::ofstream f_cnt;
	int out_cnt_0 = 0 - RANDOM_SS ;
	int out_cnt_1 = RANDOM_SS;
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

/* reset threshold after each update */
void reset_threshold(int party){
	std::ofstream f_cnt;
	int out_cnt_0 = DEBUG_NOISY_TH - RANDOM_SS ;
	int out_cnt_1 = RANDOM_SS;
	if (party == ALICE){
		std::string fpath = obj_path + "th_0";
		f_cnt.open (fpath, std::ios::trunc);
		f_cnt << out_cnt_0  << "\n";
		f_cnt.close();
	}
	else{
		std::string fpath = obj_path + "th_1";	
		f_cnt.open (fpath, std::ios::trunc);
		f_cnt << out_cnt_1  << "\n";
		f_cnt.close();
	}
	
}

#endif
