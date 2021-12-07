#include <random> 
#include <chrono> 
#include <stdlib.h>
#include  "hotstuff/quorums.h"
#include "hotstuff/crypto.h"
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/matrix.h>
#include "algebra_utils.h"

using namespace NTL;
using namespace std;


namespace ThresholdCoin{
    long t,n;
    PrimeOrderModGroup G;

    void init(const PrimeOrderModGroup& group, const long& threshold, const long& n_parties){
        G = group;
        t = threshold;
        n = n_parties;
    }

    // Returns s, the shares of a random secret x,
    // V0 = g^x, the 'verification' of the secret,
    // V, the verification keys, Vi = g^si
    // The function also returns the secret x only for testing purposes.
    void keyGen(vec_ZZ_p& s, ZZ_p& V0, vec_ZZ_p& V, ZZ_p& secret){
        ZZ_p::init(G.q);
        ZZ_pX f;
        while (true){
            f = random_ZZ_pX(t); // f(x) = a_0 + a_1 * x + a_2 * x^2 + ... + a_{t-1} * x^{t-1}, where a_0 = secret
            if (deg(f) == t-1) break;
        }
        // cout << "Polynomial f: " << f << endl;
        clear(s); s.SetLength(n); 
        for (long j = 0; j < n; j++){ //party 0 gets f(1), ..., party i gets f(i+1)
            ZZ_p point = ZZ_p(j+1);
            s[j] = eval(f, point);
        }
        secret = coeff(f, 0);
        ZZ_p::init(G.p);
        V0 = power(G.g, rep(secret));
        clear(V); V.SetLength(n);
        for (long j = 0; j < n; j++){
            V[j] = power(G.g, rep(s[j]));
        }    
    }

    void generateCoinShare(std::string C, long j, ZZ_p sj, ZZ_p& coinj, ValidityProof& proofj){
        ZZ_p::init(G.q);
        ZZ_p r = NTL::random_ZZ_p();
        
        ZZ_p::init(G.p);
        ZZ_p gtilde = H(C, G);
        ZZ_p gtildej = power(gtilde, rep(sj));
        coinj = gtildej;
        ZZ_p gj = power(G.g, rep(sj));
        ZZ_p hj = power(G.g, rep(r));
        ZZ_p htildej = power(gtilde, rep(r));

        ZZ_p::init(G.q);
        ZZ_p c = H_prime(G.g, gj, hj, gtilde, gtildej, htildej, G);
        ZZ_p z = r - (sj * c); //!!!
        proofj = ValidityProof{c, z};
    }

    bool verify(std::string C, long j, ZZ_p Vj, ZZ_p coinj, ValidityProof proofj){
        // gj is Vj, gtildej is coinj
        ZZ_p::init(G.p);
        ZZ_p gtilde = H(C, G);
        ZZ_p hj = power(G.g, rep(proofj.z)) * power(Vj, rep(proofj.c));
        ZZ_p htildej = power(gtilde, rep(proofj.z)) * power(coinj, rep(proofj.c));
        return (H_prime(G.g, Vj, hj, gtilde, coinj, htildej, G) == proofj.c);
    }

    // x and y are t pairs, such that f(xj) = yj
    // Returns the corresponding Lagrange coefficients at point 0
    vec_ZZ_p getLagrange(const vec_ZZ_p& x){
        ZZ_p::init(G.q);
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

    ZZ_p combine(const std::unordered_set<hotstuff::ReplicaID>& A, const vec_ZZ_p& coin_shares){ //For testing only, pass all shares and use only those owned by parties in A
        // each coin_shares[j] is gtildej
        ZZ_p::init(G.q);
        vec_ZZ_p indexes_A;
        for (int j = 0; j < n; j++){
            if (A.count(j) > 0){ // Party j is in A. Party j has the share f(j+1).
                indexes_A.append(ZZ_p(j+1));
            }
        }
        vec_ZZ_p lagrange_coefs = getLagrange(indexes_A);
        
        ZZ_p::init(G.p);
        vec_ZZ_p values_A;
        for (int j = 0; j < n; j++){
            if (A.count(j) > 0){ 
                values_A.append(coin_shares[j]);
            }
        }
        ZZ_p res = ZZ_p(1);
        for (long k = 0; k < indexes_A.length(); k++){
            res *= power(values_A[k], rep(lagrange_coefs[k]));
        }
        return res;
    }
}


namespace GeneralizedCoin{
    hotstuff::quorums::Msp msp;
    PrimeOrderModGroup G;

    void init(const PrimeOrderModGroup& group, const string& quorumsConfFile){
        G = group;
        ZZ_p::init(G.q);
        hotstuff::quorums::JsonParser *jp = new hotstuff::quorums::ConfigJsonParser();
        hotstuff::quorums::AccessStructure accessStructure(jp);
        accessStructure.initialize(quorumsConfFile);
        msp = accessStructure.msp;
    }

    // Returns s, the shares of a random secret x,
    // V0 = g^x, the 'verification' of the secret,
    // V, the verification keys, Vi = g^si
    // The function also returns the secret x only for testing purposes.
    void keyGen(vec_ZZ_p& s, ZZ_p& V0, vec_ZZ_p& V, ZZ_p& secret){
        ZZ_p::init(G.q);
        vec_ZZ_p r = random_vec_ZZ_p(msp.d());
        s = msp.M * r;
        secret = r[0];
        ZZ_p::init(G.p);
        V0 = power(G.g, rep(secret));
        clear(V); V.SetLength(msp.m());
        for (long j = 0; j < msp.m(); j++){
            V[j] = power(G.g, rep(s[j]));
        }
        // cout << "Sharing finished :" << endl;    
    }

    //exactly the  same  as in threshold coin
    void generateCoinShare(std::string C, long j, ZZ_p sj, ZZ_p& coinj, ValidityProof& proofj){
        ZZ_p::init(G.q);
        ZZ_p r = NTL::random_ZZ_p();
        
        ZZ_p::init(G.p);
        ZZ_p gtilde = H(C, G);
        ZZ_p gtildej = power(gtilde, rep(sj));
        coinj = gtildej;
        ZZ_p gj = power(G.g, rep(sj));
        ZZ_p hj = power(G.g, rep(r));
        ZZ_p htildej = power(gtilde, rep(r));

        ZZ_p::init(G.q);
        ZZ_p c = H_prime(G.g, gj, hj, gtilde, gtildej, htildej, G);
        ZZ_p z = r - (sj * c); //!!!
        proofj = ValidityProof{c, z};
    }

    //exactly the  same  as in threshold coin
    bool verify(std::string C, long j, ZZ_p Vj, ZZ_p coinj, ValidityProof proofj){
        // gj is Vj, gtildej is coinj
        ZZ_p::init(G.p);
        ZZ_p gtilde = H(C, G);
        ZZ_p hj = power(G.g, rep(proofj.z)) * power(Vj, rep(proofj.c));
        ZZ_p htildej = power(gtilde, rep(proofj.z)) * power(coinj, rep(proofj.c));
        return (H_prime(G.g, Vj, hj, gtilde, coinj, htildej, G) == proofj.c);
    }

    ZZ_p combine(const std::unordered_set<hotstuff::ReplicaID>& A, const vec_ZZ_p& coin_shares){ //For testing only, pass all shares and use only those owned by parties in A
        // each coin_shares[j] is gtildej
        ZZ_p::init(G.q);
        vec_ZZ_p lambda_A = msp.getRecombinationVector(A);
        
        ZZ_p::init(G.p);
        vec_ZZ_p coin_shares_A;
        for (int j = 0; j < coin_shares.length(); j++){
            if (A.count(msp.L[j]) > 0){
                coin_shares_A.append(coin_shares[j]);
            }
        }
        ZZ_p res = ZZ_p(1);
        for (long k = 0; k < coin_shares_A.length(); k++){
            res *= power(coin_shares_A[k], rep(lambda_A[k]));
        }
        return res;
    }
}


int benchThreshold(PrimeOrderModGroup G, long t, long n, string marker, int repetitions = 1){
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::duration<double, std::milli> elapsedKeyGen = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedGenerateCoin = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerify = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedCombine = std::chrono::milliseconds(0);
    
    for (int i = 0; i < repetitions; i++){
        //init 
        ThresholdCoin::init(G, t, n);

        std::string C = "coin" + std::to_string(i);
    
        ZZ_p::init(G.q);
        vec_ZZ_p s;
        ZZ_p secret;
        vector<ValidityProof> validity_proofs(ThresholdCoin::n);
        
        ZZ_p::init(G.p);
        ZZ_p gtilde = H(C, G);
        ZZ_p V0;
        vec_ZZ_p V;
        vec_ZZ_p coin_shares;
        coin_shares.SetLength(ThresholdCoin::n);

        //generate keys        
        begin = std::chrono::steady_clock::now();
        ThresholdCoin::keyGen(s, V0, V, secret);
        end = std::chrono::steady_clock::now();
        elapsedKeyGen += end - begin;

        //generate coin shares
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < ThresholdCoin::n; j++){
            ThresholdCoin::generateCoinShare(C, j, s[j], coin_shares[j], validity_proofs[j]);
        }
        end = std::chrono::steady_clock::now();
        elapsedGenerateCoin += (end - begin) / ThresholdCoin::n; //average time it takes a party to verify one share

        //verify
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < ThresholdCoin::n; j++){
            if (!ThresholdCoin::verify(C, j, V[j], coin_shares[j], validity_proofs[j])){
                cout << "Problem: " << j << endl;
                return 1;
            }
        }
        end = std::chrono::steady_clock::now();
        elapsedVerify += (end - begin) / ThresholdCoin::n; //average time it takes a party to verify one share

        //combine
        std::vector<hotstuff::ReplicaID> S;
        for (long i = 0; i < ThresholdCoin::n; i++){
            S.push_back(i);
        }
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        auto gen = std::default_random_engine(seed);
        shuffle(S.begin(), S.end(), gen);
        std::unordered_set<hotstuff::ReplicaID> A;
        for (long i = 0; i < t; i++){
            A.insert(S.at(i));
        }
        // cout << "size " << A.size() << endl;

        ZZ_p::init(G.p);
        begin = std::chrono::steady_clock::now();
        ZZ_p coin_value = ThresholdCoin::combine(A, coin_shares);
        end = std::chrono::steady_clock::now();
        elapsedCombine += end - begin;
        if (coin_value != power(gtilde, rep(secret))){
            cout << "Problem in combine: " << coin_value << endl;
            return 1;
        }
    }

    cout << marker << "." << n << ".KEYGEN. " << elapsedKeyGen.count() / repetitions << endl;
    cout << marker << "." << n << ".GENER. " << elapsedGenerateCoin.count() / repetitions << endl;
    cout << marker << "." << n << ".VERIF. " << elapsedVerify.count() / repetitions << endl;
    cout << marker << "." << n << ".COMB. " << elapsedCombine.count() / repetitions << endl;
    return 0;
}


int benchGeneralized(PrimeOrderModGroup G, const string& quorumsConfFile, long n, string marker, int repetitions = 1){    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::duration<double, std::milli> elapsedKeyGen = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedGenerateCoin = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerify = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedCombine = std::chrono::milliseconds(0);
    
    for (int i = 0; i < repetitions; i++){
        //init
        GeneralizedCoin::init(G, quorumsConfFile);

        std::string C = "coin" + std::to_string(i);

        ZZ_p::init(G.q);
        vec_ZZ_p s;
        ZZ_p secret;
        vector<ValidityProof> validity_proofs(GeneralizedCoin::msp.m());
        
        ZZ_p::init(G.p);
        ZZ_p gtilde = H(C, G);
        ZZ_p V0;
        vec_ZZ_p V;
        vec_ZZ_p coin_shares;
        coin_shares.SetLength(GeneralizedCoin::msp.m());

        //generate keys        
        begin = std::chrono::steady_clock::now();
        GeneralizedCoin::keyGen(s, V0, V, secret);
        end = std::chrono::steady_clock::now();
        elapsedKeyGen += end - begin;

        //generate coin shares
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < GeneralizedCoin::msp.m(); j++){
            GeneralizedCoin::generateCoinShare(C, j, s[j], coin_shares[j], validity_proofs[j]);
        }
        end = std::chrono::steady_clock::now();
        elapsedGenerateCoin += (end - begin) / GeneralizedCoin::msp.m(); //average time it takes a party to verify one share

        //verify
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < GeneralizedCoin::msp.m(); j++){
            if (!GeneralizedCoin::verify(C, j, V[j], coin_shares[j], validity_proofs[j])){
                cout << "Problem: " << j << endl;
                return 1;
            }
        }
        end = std::chrono::steady_clock::now();
        elapsedVerify += (end - begin) / GeneralizedCoin::msp.m(); //average time it takes a party to verify one share

        //combine
        std::vector<hotstuff::ReplicaID> S;
        for (long i = 0; i < n; i++){
            S.push_back(i);
        }
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        auto gen = std::default_random_engine(seed);
        shuffle(S.begin(), S.end(), gen);
        std::unordered_set<hotstuff::ReplicaID> A;
        while (true){
            int A_size = rand()%n + 1;
            for (long i = 0; i < A_size; i++){
                A.insert(S.at(i));
            }
            if (GeneralizedCoin::msp.isAuthorizedGroup(A)){
                break;
            }
        }
        // cout << "size " << A.size() << endl;

        ZZ_p::init(G.p);
        begin = std::chrono::steady_clock::now();
        ZZ_p coin_value = GeneralizedCoin::combine(A, coin_shares);
        end = std::chrono::steady_clock::now();
        elapsedCombine += end - begin;
        if (coin_value != power(gtilde, rep(secret))){
            cout << "Problem in combine: " << coin_value << endl;
            return 1;
        }

    }

    cout << marker << "." << n << ".KEYGEN. " << elapsedKeyGen.count() / repetitions << endl;
    cout << marker << "." << n << ".GENER. " << elapsedGenerateCoin.count() / repetitions << endl;
    cout << marker << "." << n << ".VERIF. " << elapsedVerify.count() / repetitions << endl;
    cout << marker << "." << n << ".COMB. " << elapsedCombine.count() / repetitions << endl;
}


int test_hash_functions(){
    long p_size = 3072; //3072
    long q_size = 256;  //256
    PrimeOrderModGroup G = getGroup(p_size,q_size);
    
    unsigned char* p;
    long size;

    ZZ_p::init(G.p);    
    ZZ_p h;
    for (std::string str : {"abc", "test", "test.", "", "crypto", "16", "255"}){
        p = (unsigned char*)str.c_str();
        size = strlen(str.c_str());
        cout << str << ":" << endl;
        for (int i = 0; i < size; i++)
            printf("%02x", p[i]);
        printf("\n");
        cout << size << endl;
        h = H(str, G);
    }

    ZZ_p::init(G.q);
    for (std::string str : {"abc", "test", "test.", "", "crypto", "16", "255"}){
        p = (unsigned char*)str.c_str();
        size = strlen(str.c_str());
        cout << str << ":"  << endl;
        for (int i = 0; i < size; i++)
            printf("%02x", p[i]);
        printf("\n");
        cout << size << endl;
        unsigned char* md_value;
        unsigned int md_len;
        digest_message(p, size, &md_value, &md_len);
        printf("Digest is: ");
        for (int i = 0; i < md_len; i++)
            printf("%02x", md_value[i]);
        printf("\n");
    }

    for (ZZ_p h : {ZZ_p(16), ZZ_p(255), ZZ_p(256), ZZ_p(487), ZZ_p(569856), ZZ_p(16), ZZ_p(998651944684384313994671330456640684), ZZ_p(48)}){
        //Decimal 45 = 0x30 = ASCII of "0"
        bytesFromZZp(h, &p, &size);
        for (int i = 0; i < size; i++)
            printf("%02x", p[i]);
        printf("\n");
        cout << size << endl;
        unsigned char* md_value;
        unsigned int md_len;
        digest_message(p, size, &md_value, &md_len);
        printf("Digest is: ");
        for (int i = 0; i < md_len; i++)
            printf("%02x", md_value[i]);
        printf("\n");
    }

    ZZ_p x = H_prime(ZZ_p(0), ZZ_p(1), ZZ_p(2), ZZ_p(16), ZZ_p(254), ZZ_p(256), G);
    cout << x << endl;

    return 0;
}

int main(){    
    ZZ p,q;
    long p_size = 3072; //3072
    long q_size = 256;  //256
    // generatePrimes(p_size, q_size, p, q);
    PrimeOrderModGroup G = getGroup(p_size,q_size);
    
    int from = 3;
    int to = 101;
    int step = 8;
    int repetitions = 10;
    for (int n = from; n <= to; n+=step){
        benchThreshold(G, n / 2 + 1, n, "THR", repetitions);
    }
    for (int n = from; n <= to; n+=step){
        std::string quorumSpec = "conf/quorum_confs/quorums_maj_thres_" + std::to_string(n) + ".json";
        benchGeneralized(G, quorumSpec, n, "GEN", repetitions);
    }
    for (int n = from; n <= to; n+=step){
        benchThreshold(G, 2*n/3 + 1, n, "THRCOMPL", repetitions);
    }
    for (int n = from; n <= to; n+=step){
        std::string quorumSpec = "conf/quorum_confs/quorums_thres_" + std::to_string(n) + ".json";
        benchGeneralized(G, quorumSpec, n, "GENCOMPL", repetitions);
    }
    
   return 0;
}
