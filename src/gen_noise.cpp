#include "emp-sh2pc/emp-sh2pc.h"
#include <cmath>
#include <chrono>

#define PRECISION   8

using namespace emp;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

// Function for test x^2 + y^2
void test_square_sum(int party, int number){
    Integer a(32, number, ALICE);
	Integer b(32, number, BOB);
    Integer res(32, -1, BOB);
    res = a*a + b*b;
    cout << "x^2 + y^2 =\t"<< res.reveal<int32_t>()<<endl;
}

/* Function to generate secret share
    1. Each party inputs a random bit string z_b
    2. Compute z = z_1 ^ z_0
    3. Generate two shares with sh1 = z ^ val and sh2 = z
*/
void generate_secret_share(int party, int number, int val){
    Integer a(32, number, ALICE);
	Integer b(32, number, BOB);
    Integer vnum(32, val, BOB);
    Integer sh1(32, -1, BOB);
    Integer sh2(32, -1, ALICE);
    sh1 = a^b^vnum;
    sh2 = a^b;
    cout << "Secret share for Bob is"<< sh1.reveal<int32_t>(BOB)<<endl;
    cout << "Secret share for Alice is"<< sh2.reveal<int32_t>(ALICE)<<endl;
}

/* Function to recover secret share
    1. Each party inputs a secret sh_b
    2. Compute val = sh1 ^ sh0
*/
void recover_secret_share(int party, int number){
    Integer a(32, number, ALICE);
	Integer b(32, number, BOB);
    Integer res(32, -1, PUBLIC);
    res = a^b;
    cout << "The recovered value is: "<< res.reveal<int32_t>()<<endl;
}

#ifdef SECURE_PRG
/* Lplace noise generation using secure PRG*/
// Test only purpose
Float sprg_laplace_noise(int party, int number, float eps = 1.0, int sens = 1){
    float epsilon = eps;
    int sensitivity = sens;
    float coeff = sensitivity / epsilon;
    int sign = -1;

    float nlaplace = 0.0;
    PRG prg;    //using a random seed
	block rand_block[3];
	prg.random_block(rand_block, 3);
	prg.reseed(&rand_block[1]);//reset the PRG with receovered seed

    int ia = 0;
	prg.random_data(&ia, 4);
    float da = (float)(ia) / RAND_MAX;
    Float na = Float(da, PUBLIC);
    Float res = Float(0.0, PUBLIC);
    res = na.abs().ln();
    nlaplace = sign * coeff * res.reveal<double>(PUBLIC);
    return nlaplace;
}
#endif


/* Function to generate Laplace noise with truncation convert method

    1. Each party inputs a random bin string z_b
    2. Compute rseed = z0 + z1 (or z0 ^ z1)
    3. Convert the rseed to a 32 bit float r in (0,1)
    4. compute ln(abs(r)) * (sensitivity / epsilon) * sign

    Test command: ./bin/[this_bin_name] 1 [port] [rdNum1] & ./bin/[this_bin_name] 2 [port] [rdNum2]
*/
Float tc_laplace_noise(int party, int number, float eps = 1.0, int sens = 1){
    
    float epsilon = eps;
    int sensitivity = sens;
    float coeff = sensitivity / epsilon;
    int sign = -1;
    Float s_coeff = Float(coeff, PUBLIC);
    Float s_sign = Float((float)sign, PUBLIC);
    Float mask = Float(0.5);

    Integer a(32, number, ALICE);
	Integer b(32, number, BOB); 
    Integer zero(32, 0, PUBLIC);
    Integer neg(32, -1, PUBLIC);
    Integer rseed(32, -1);
    Float f_neg(-1);
    
    rseed = a ^ b;                       // can be replaced as a + b
    Bit gt = rseed > zero;
    rseed = If(gt, rseed, rseed * neg);
    s_sign = If(gt, f_neg * s_sign, s_sign);
    /* convert to fixed-point value in (0,1)*/

#ifdef SIMPLE_CLIP
    /* approach 1 simple convert by mod */
    Integer clp(32, 1065353216, PUBLIC); // Hex 3F800000 for Decimal 1065353216 
    rseed = rseed % clp;
    cout << "The rseed is : "<< rseed.reveal<int>(PUBLIC) << endl;
    Float val(0.0, PUBLIC);
    memcpy(val.value.data(), rseed.bits.data(), 32*sizeof(block));
    std::cout << "The randomness is " << val.reveal<double>() << std::endl;
#else
    /* truncate convert */
    /* 
        By default the laplace noise converting is done over fixed-point numbers.
        The following provides an alternative way over floating point numbers (please refer to sc_laplace_noise).

        1. set the sign bit as 0, and the exponent bits as b001111110
            - this guarantees to have a uniform random floating point number in (1,0.5)
        2. then use one additional bit to decide whether -0.5 or -0 for the resulting number.
            - this gives us a uniform random floating point number in (1,0)
    */
    Bit *cond_before_overwride = rseed.bits.data();                                            
    Bit exp_bits[9] = {Bit(0), Bit(1), Bit(1), Bit(1), Bit(1), Bit(1), Bit(1), Bit(0), Bit(0)}; //little endian storage
    memcpy(rseed.bits.data()+23, exp_bits, 9*sizeof(block));

    Float val(0.0, PUBLIC);
    memcpy(val.value.data(), rseed.bits.data(), 32*sizeof(block));
    val = If(cond_before_overwride[0], val, val - mask);
    std::cout << "The randomness is " << val.reveal<double>() << std::endl;
#endif

    /* transform to Laplace noise */
    val = val.abs().ln();
    val = val * s_coeff;
    val = val * s_sign;
    cout << "The noise is: "<< val.reveal<double>(BOB) << endl;
    return val;
}

/* Function to generate Laplace noise with simple convert method 

    1. Each party inputs a random bin string z_b
    2. Compute rseed = z0 + z1 (or z0 ^ z1)
    3. Treat the rseed to as a 32 bit fixed-point number in (0,1) with precision d.
    4. convert rseed to a Float type r with the same value.
    5. compute ln(abs(r)) * (sensitivity / epsilon) * sign
*/
Float sc_laplace_noise(int party, int number, float eps = 1.0, int sens = 1){
    float epsilon = eps;
    int sensitivity = sens;
    float coeff = sensitivity / epsilon;
    int sign = -1;
    int precision = PRECISION;
    Float s_coeff = Float(coeff, PUBLIC);
    Float s_sign = Float((float)sign, PUBLIC);
    Float r = Float(0.0);
    Float f_neg(-1);
    Float base(2.0);
    Float f1(1.0);
    Float f0(0.0);

    Integer a(32, number, ALICE);
	Integer b(32, number, BOB); 
    Integer zero(32, 0, PUBLIC);
    Integer neg(32, -1, PUBLIC);
    Integer rseed(32, -1);
    
    rseed = a ^ b;                       // can be replaced as a + b
    Bit gt = rseed > zero;
    rseed = If(gt, rseed, rseed * neg);
    s_sign = If(gt, f_neg * s_sign, s_sign);

    Bit *fixed_rseed = rseed.bits.data();
    //Bit fixed_rseed[10] = {Bit(1), Bit(1), Bit(0), Bit(1), Bit(1), Bit(1), Bit(1), Bit(1), Bit(1), Bit(1)}; //little endian storage
    /* 
        starting from the MSB of fixed_rseed, compute r as sum_{i=0}^{31-precision} b_i x 2^{-i-1} 
        as long as a, b are uniform binary string, then r is uniform distributed over range (0,1)
    */
    for (int i = 32 - precision - 1; i < 32; i++ ){
        r = r / base;
        r = r + If(fixed_rseed[i], f1, f0);
    }
    r = r / base;
    //std:cout << "The randomness is " << r.reveal<double>() << std::endl;

    /* transform to Laplace noise */
    r = r.abs().ln();
    r = r * s_coeff;
    r = r * s_sign;
    cout << "The noise is: "<< r.reveal<double>() << endl;
    return r;

}


int main(int argc, char** argv) {
	int port, party;
    string fileName = argv[3];
	parse_party_and_port(argv, &party, &port);
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    // read inputs from external file.
    fstream sharesFile;
    std::vector<int> vect;
	string fullPath = "/home/ypzhang0725/mpc/emp-sh2pc/";
	
    sharesFile.open(fullPath + fileName, ios::in); //open a file to perform read operation using file object
    if (sharesFile.is_open()){   //checking whether the file is open
    string tp;
    while(getline(sharesFile, tp)){ //read data from file object and put it into string.
                        std::stringstream ss(tp);

                        for (int i; ss >> i;) {
                                vect.push_back(i);
                                if (ss.peek() == ',')
                                        ss.ignore();
                        }
                //cout << tp << "\n"; //print the data of the string
        }
        sharesFile.close(); //close the file object.
   }

	setup_semi_honest(io, party);
    Float z(0.0, PUBLIC);
	//z = tc_laplace_noise(party, atoi(argv[3]), 1.0, 1);
    z = sc_laplace_noise(party, atoi(argv[3]), 1.0, 1);
	cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
