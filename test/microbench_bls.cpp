#include <random> 
#include <chrono> 
#include <stdlib.h>
#include "hotstuff/crypto.h"

uint256_t getRandomBlockHash(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    uint8_t hash[32];
    for (int i = 0; i < 32; i++){
        hash[i] = rand() % 256;
    }
    return hash;
}


int main(){
    bls::init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);

    std::chrono::duration<double, std::micro> elapsedSignBls = std::chrono::microseconds(0);
    std::chrono::duration<double, std::micro> elapsedVerifyBls = std::chrono::microseconds(0);
    std::chrono::duration<double, std::micro> elapsedSignEcdsa = std::chrono::microseconds(0);
    std::chrono::duration<double, std::micro> elapsedVerifyEcdsa = std::chrono::microseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    int repetitions = 1000;
    for (int i = 0; i < repetitions; i++)
    {
        //GET RANDOM MESSAGE AND KEYS
        uint256_t hash = getRandomBlockHash();
        hotstuff::PrivKeyBls sk_bls;
        sk_bls.from_rand();
        hotstuff::pubkey_bt pk_bls = sk_bls.get_pubkey();
        hotstuff::PrivKeySecp256k1 sk_ecdsa;
        sk_ecdsa.from_rand();
        hotstuff::pubkey_bt pk_ecdsa = sk_ecdsa.get_pubkey();
        // std::cout << "Hash: " << get_hex(hash).c_str() << " Private Key: " << get_hex(sk_bls).c_str() << " Public Key: " << get_hex(*pk_bls).c_str() << std::endl;
        
        //SIGN with BLS
        begin = std::chrono::steady_clock::now();
        hotstuff::PartCertBls partCertBls = hotstuff::PartCertBls(sk_bls, hash);
        end = std::chrono::steady_clock::now();
        elapsedSignBls += end - begin;
        
        //SIGN with ECDSA
        begin = std::chrono::steady_clock::now();
        hotstuff::PartCertSecp256k1 partCertEcdsa = hotstuff::PartCertSecp256k1(sk_ecdsa, hash);
        end = std::chrono::steady_clock::now();
        elapsedSignEcdsa += end - begin;

        //VERIFY with BLS
        begin = std::chrono::steady_clock::now();
        if (partCertBls.verify(*pk_bls) == false) return 1;
        end = std::chrono::steady_clock::now();
        elapsedVerifyBls += end - begin;

        //VERIFY with ECDSA
        begin = std::chrono::steady_clock::now();
        if (partCertEcdsa.verify(*pk_ecdsa) == false) return 1;
        end = std::chrono::steady_clock::now();
        elapsedVerifyEcdsa += end - begin;
    }
    
    std::cout << "BLS_SIGN." << elapsedSignBls.count() / repetitions << std::endl; // time in microseconds
    std::cout << "BLS_VERIFY." << elapsedVerifyBls.count() / repetitions << std::endl; // time in microseconds
    std::cout << "ECDSA_SIGN." << elapsedSignEcdsa.count() / repetitions << std::endl; // time in microseconds
    std::cout << "ECDSA_VERIFY." << elapsedVerifyEcdsa.count() / repetitions << std::endl; // time in microseconds
    return 0;
}