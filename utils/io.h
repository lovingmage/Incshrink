#ifndef IO_H__
#define IO_H__

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

#endif
