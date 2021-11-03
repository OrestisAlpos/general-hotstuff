#include <random> 
#include <chrono> 
#include <stdlib.h>
#include <bls/bls384_256.h>
#include <bls/bls.hpp>
#include <iostream>
#include <fstream>

std::string getRandomMessage(int length){
    static std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    std::string msg;
    msg.resize(length);
    for (int i = 0; i < length; i++){
        msg[i] = charset[rand() % charset.length()];
    }
    return msg;
}

int main(){

    bls::SecretKey sk;
    bls::PublicKey pk;
    bls::Signature sig;
    std::string msg;

    std::chrono::duration<double, std::milli> elapsedSign = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerify = std::chrono::milliseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::string res = "Reporting results in milliseconds\n";

    // MCL_BN254, MCL_BN381_1, MCL_BN381_2, MCL_BN462, MCL_BN_SNARK1, MCL_BLS12_381, MCL_BN160
    //  MCL_BN381_1, MCL_BN381_2,  MCL_BN462: Require higher MCLBN_FP_UNIT_SIZE value, was not able to make them work.
    for ( const auto curve : {MCL_BN254, MCL_BN_SNARK1, MCL_BLS12_381, MCL_BN160} ){
        bls::init(curve, MCLBN_COMPILED_TIME_VAR);

        int repetitions = 1000; 
        for (int i = 0; i < repetitions; i++)
        {
            //GET RANDOM MESSAGE AND KEYS
            sk.init();
            sk.getPublicKey(pk);
            msg = getRandomMessage(32);
            
            //SIGN
            begin = std::chrono::steady_clock::now();
            sk.sign(sig, msg);
            end = std::chrono::steady_clock::now();
            elapsedSign += end - begin;

            //VERIFY
            begin = std::chrono::steady_clock::now();
            if (!sig.verify(pk, msg)) return 1;
            end = std::chrono::steady_clock::now();
            elapsedVerify += end - begin;
        }
        
        res.append("BLS-" + std::to_string(curve) + " SIGN: " + std::to_string(elapsedSign.count() / repetitions) + "\n");
        res.append("BLS-" + std::to_string(curve) + " VERIFY: " + std::to_string(elapsedVerify.count() / repetitions) +  "\n"); 
        elapsedSign = std::chrono::milliseconds(0);
        elapsedVerify = std::chrono::milliseconds(0);
    }  
	std::ofstream ofs("test/microbench_pairings.txt");
	if (!(ofs << res)) return 1;
    
    std::cout << res;
    return 0;
}