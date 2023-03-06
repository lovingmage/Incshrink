#include "emp-sh2pc/emp-sh2pc.h"
#include <utils/gen_noise.h>
#include <cmath>
#include <chrono>

#define PRECISION   8

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
	Integer z(32, 0, PUBLIC);
    z = sprg_laplace_noise(party, 1, 0.1, 1);
    cout << z.reveal<int>(PUBLIC)<<endl;
	//cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
