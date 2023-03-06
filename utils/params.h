#ifndef PARAMS_H__
#define PARAMS_H__

#define DUMMY_THRESHOLD 100
#define RANDOM_SS -10
#define DEBUG_NOISY_TH 100

string obj_path = "/Users/lovingmage/Desktop/dev-pool/incshrink/incshrink/data/";
// Configurations
string full_path = "/Users/lovingmage/Desktop/dev-pool/incshrink/incshrink/data/";

// Define Data Structure
class Data {public:
    Integer * data;
    uint32_t public_size;
	uint32_t fpos;
	uint32_t r_size;
	std::vector<int32_t> attr;
    Integer real_size; // Reserved for Dubug
};

#endif