#ifndef MEMORY_H__
#define MEMORY_H__

using namespace emp;
using namespace std;

// concantenate two emp::Integer memory blocks
void memconcat(Integer * res, Integer * str0, Integer * str1, uint32_t sz0, uint32_t sz1){
	//Integer * res = new Integer[sz0 + sz1];
	std::memcpy(res, str0, (sz0) * sizeof(Integer));
	std::memcpy(res+(sz0), str1, (sz1) * sizeof(Integer));
}



//test of concantenation
void test_concat(){
	Integer a(32, 0, PUBLIC);
	Integer b(32, 1, PUBLIC);
	Integer c(32, 2, PUBLIC);
	Integer d(32, 3, PUBLIC);
	Integer e(32, 4, PUBLIC);

	Integer left[3] = {a,b,c};
	Integer right[2] = {d, e};
	Integer * conc = new Integer[5];
	
	memconcat(conc, left, right, 3, 2);
	std::cout << "Test concant " << std::endl;
	std::cout << conc[0].reveal<int>() << std::endl;
	std::cout << conc[1].reveal<int>() << std::endl;
	std::cout << conc[2].reveal<int>() << std::endl;
	std::cout << conc[3].reveal<int>() << std::endl;
	std::cout << conc[4].reveal<int>() << std::endl;
}

#endif
