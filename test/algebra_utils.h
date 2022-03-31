#include <random> 
#include <chrono> 
#include <stdlib.h>
#include  "hotstuff/quorums.h"
#include "hotstuff/crypto.h"
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/matrix.h>
#include <openssl/evp.h>

using namespace NTL;
using namespace std;

//group G of prime order q, subgroup of Zp, with g, h generators
struct PrimeOrderModGroup {
    ZZ p;
    ZZ q;
    ZZ_p g;
    ZZ_p h;
};

// Generate primes p,q, such that p-1 divides q, i.e., p - 1 = mq
void generatePrimes(long p_size, long q_size, ZZ& p, ZZ& q){
    while (true){
        q = GenPrime_ZZ(q_size);
        ZZ m1 = RandomLen_ZZ(p_size - q_size);
        ZZ m2 = RandomLen_ZZ(p_size - q_size);
        if (m1 > m2){
            ZZ temp = m1;
            m1 = m2;
            m2 = temp;
        }
        // cout << "***" << m1 << endl;
        // cout << "***" << m2 << endl;
        for (ZZ m = m1; m < m2; m++){
            p = q * m + 1;
            if (ProbPrime(p)){
                // cout << "***" << q << endl;
                // cout << "***" << p << endl;
                return ;
            }    
        }
    }
}


// Generation of system parameters.
// Returns (p, q, g, h), where g, h are generators of the subroup of order q of Zp
const PrimeOrderModGroup getGroup(long p_size, long q_size){ 
    ZZ p,q;
    generatePrimes(p_size, q_size, p, q);
    // cout << "Primes: " << p << " " << q << endl;
    ZZ_p::init(p);
    ZZ m = (p - 1) / q;
    ZZ_p g;
    ZZ_p h;
    while (true) {
        ZZ_p g1 = NTL::random_ZZ_p();
        ZZ_p h1 = NTL::random_ZZ_p();
        g = NTL::power(g1, m);
        h = NTL::power(h1, m);
        if (!NTL::IsOne(g) && !NTL::IsOne(h))
            break;
    }
    return PrimeOrderModGroup{p,q,g,h};
}

struct ValidityProof{
    ZZ_p c;
    ZZ_p z;
};


int digest_message(const unsigned char *message, size_t message_len, unsigned char **digest, unsigned int *digest_len)
{
	EVP_MD_CTX *mdctx;

	if((mdctx = EVP_MD_CTX_new()) == NULL)
		return 1;

	if(1 != EVP_DigestInit_ex(mdctx, EVP_sha512(), NULL))
		return 1;

	if(1 != EVP_DigestUpdate(mdctx, message, message_len))
		return 1;

	if((*digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha512()))) == NULL)
		return 1;

	if(1 != EVP_DigestFinal_ex(mdctx, *digest, digest_len))
		return 1;

	EVP_MD_CTX_free(mdctx);
    return 0;
}

// H: {0, 1}^* -> G, where G is a subroup of Zp with prime order q.
// This function assumes q is a 256-bit number and uses SHA512. 
// Thus, no expansion is required before reducing the output of SHA512 mod q.
ZZ_p H(std::string message, PrimeOrderModGroup G){
    unsigned char* p = (unsigned char*)message.c_str();
    long p_size = strlen(message.c_str());
    unsigned char* md_value;
    unsigned int md_len;
    digest_message(p, p_size, &md_value, &md_len);
    // printf("Digest is: ");
    // for (int i = 0; i < md_len; i++)
    //     printf("%02x", md_value[i]);
    // printf("\n");
    // cout << md_len << endl;
    ZZ e = ZZFromBytes(md_value, md_len);
    e = AddMod(e, ZZ(1), G.q);
    ZZ_p::init(G.p);
    return power(G.g, e);
}

void bytesFromZZp(ZZ_p h, unsigned char** p, long* size){
    ZZ h_rep = rep(h);
    *size = NumBytes(h_rep);
    Vec<unsigned char> buf;
    buf.SetLength(*size);
    BytesFromZZ(buf.elts(), h_rep, *size);
    *p = buf.elts();
}

// H': G^6 -> Zq
// This function assumes q is a 256-bit number and uses SHA512. 
// Thus, no expansion is required before reducing the output of SHA512 mod q.
ZZ_p H_prime(ZZ_p h1, ZZ_p h2, ZZ_p h3, ZZ_p h4, ZZ_p h5, ZZ_p h6, PrimeOrderModGroup G){
    
    //init sha512 digest
    EVP_MD_CTX *mdctx;
	if((mdctx = EVP_MD_CTX_new()) == NULL) throw runtime_error("EVP_MD_CTX_new") ;
	if(1 != EVP_DigestInit_ex(mdctx, EVP_sha512(), NULL)) throw runtime_error("EVP_DigestInit_ex");

    //update digest for each h1,...h6
    unsigned char* p;
    long size;
    unsigned char* md_value;
    unsigned int md_len;
    for (ZZ_p h : {h1, h2, h3, h4, h5, h6}){
        bytesFromZZp(h, &p, &size);
        digest_message(p, size, &md_value, &md_len);
        if(1 != EVP_DigestUpdate(mdctx, md_value, md_len)) throw runtime_error("EVP_DigestUpdate");
    }
    
    //finalize digest
    unsigned char* digest;
    unsigned int digest_len;
	if((digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha512()))) == NULL) throw runtime_error("EVP_MD_size");
	if(1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len)) throw runtime_error("EVP_DigestFinal_ex");
	EVP_MD_CTX_free(mdctx);

    ZZ_p::init(G.q);
    return ZZ_p({}, NTL:: ZZFromBytes(digest, digest_len));
    
}

    // x and y are t pairs, such that f(xj) = yj
    // Returns the corresponding Lagrange coefficients at point 0
    // Operations done in Zq
    vec_ZZ_p getLagrange(ZZ q, const vec_ZZ_p& x){
        ZZ_p::init(q);
        vec_ZZ_p coefs;
        coefs.SetLength(x.length());
        for (long j = 0; j < x.length(); j++){
            coefs[j] = ZZ_p(1);
            for (long k = 0; k < x.length(); k++){
                if (k == j) continue;
                coefs[j] = coefs[j] * x[k] / (x[k] - x[j]);
            }
        }
        return coefs;  
    }

// For threshold AS (like in configurations GEN, GEN_COMPL, GEN_MAJ) this will return an set with the minimu number of parties (t).
// Otherwise it keeps adding random parties to the set until it becomes authorized
std::unordered_set<hotstuff::ReplicaID> getAuthorizedSet(long n, const hotstuff::quorums::Msp& msp){
    std::vector<hotstuff::ReplicaID> all_parties;
        for (long i = 0; i < n; i++){
            all_parties.push_back(i);
        }
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        auto gen = std::default_random_engine(seed);
        shuffle(all_parties.begin(), all_parties.end(), gen);
        std::unordered_set<hotstuff::ReplicaID> auth_set;
        long j = 0;
        while (true){
            auth_set.insert(all_parties.at(j));
            j++;
            if (msp.isAuthorizedGroup(auth_set)){
                break;
            }
        }
        return auth_set;
}