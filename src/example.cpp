#include "emp-sh2pc/emp-sh2pc.h"
#include "utils/data.h"
#include "utils/oblisort.h"
using namespace emp;
using namespace std;


void test_sort(int party) {
	Integer one(32, 1, PUBLIC);
	Integer zero(32, 0, PUBLIC);

	Integer a1(32, 1, PUBLIC);
	Integer a2(32, 2, PUBLIC);
	Integer a3(32, 3, PUBLIC);
	Integer a4(32, 4, PUBLIC);
	Integer a5(32, 5, PUBLIC);

	struct data array[5] = {
    {one, {a1, a1, a1, a1, a1}},
    {zero, {a2, a2, a2, a2, a2}},
    {one, {a3, a3, a3, a3, a3}},
    {zero, {a4, a4, a4, a4, a4}},
    {one, {a5, a5, a5, a5, a5}}
};


	bitonic_sort(array, 0, 5, false);

	for(int i = 0; i < 5; ++i)
		cout << array[i].flag.reveal<int>()<< "---" <<array[i].values[0].reveal<int>()<<endl;
}

int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);
	int num = 20;
	if(argc > 3)
		num = atoi(argv[3]);
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

	setup_semi_honest(io, party);
	test_sort(party);
	//cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}