#ifndef GEN_NOISE_H__
#define GEN_NOISE_H__

#include <utils/io.h>
#include<thread>
#include <chrono>

#define RUNTIME_MAIN 0
#define VERBOSE_T 0

using namespace emp;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

/* Lplace noise generation using secure PRG*/
Integer sprg_laplace_noise(int party, int input, float eps = 1.0, int sens = 1){
    float epsilon = eps;
    int sensitivity = sens;
    float coeff = sensitivity / epsilon;

    Float sign(0.0);
	Float base(2.0);
    Float f_1(-1.0);
	Float f1(1.0);
    Float f0(0.0);
    Float mpler(coeff);

	int ia = input;

    PRG prg;    				//using a random seed
	block rand_block[3];
	prg.random_block(rand_block, 3);
	prg.reseed(&rand_block[1]); //reset the PRG with receovered seed
	prg.random_data(&ia, sizeof(ia));
    
	Integer a(32, ia, ALICE);
	Integer b(32, ia, BOB);

	Integer z = a ^ b;
    sign = If(z.bits[31], f1, f_1);
	z.bits[31] = Bit(0);		//remove sign

	Float r(0.0);
	for (int i = 0; i < 32; i++ ){
        r = r / base;
        r = r + If(z.bits[i], f1, f0);
    }
    
    //r = r.ln();

#ifdef DEBUG
	for (int i=0; i<32; i++)
		cout << z.bits[i].reveal<bool>(ALICE);

    cout << endl;
    cout << "randomness (Dec) \t" << z.reveal<int>(ALICE) << endl;
	cout << "randomness (Float 0-1) \t" << r.reveal<double>(ALICE) << endl;
    
#endif

    r = r.ln();
    r = mpler * r * sign;

#ifdef DEBUG
    cout << "ln(r) \t" << r.reveal<string>(ALICE) << endl;
#endif
    
    // should convert float to int in bitwise (current EMP do not support)
    // TODO: add util for int-float converting
    int ret = (int) r.reveal<double>();
    return Integer(32, ret, PUBLIC);
}


#endif
