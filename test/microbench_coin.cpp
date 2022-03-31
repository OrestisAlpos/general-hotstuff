#include <random> 
#include <chrono> 
#include <stdlib.h>
#include <map>
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

    //returns the coinj and proofj
    void generateCoinShare(std::string C, long j, const ZZ_p& sj, const ZZ_p& Vj, ZZ_p& coinj, ValidityProof& proofj){
        ZZ_p::init(G.q);
        ZZ_p r = NTL::random_ZZ_p();
        
        ZZ_p::init(G.p);
        ZZ_p gtilde = H(C, G);
        ZZ_p gtildej = power(gtilde, rep(sj));
        coinj = gtildej; //coin share for index j
        
        // ZZ_p gj = power(G.g, rep(sj));
        ZZ_p gj = Vj;
        ZZ_p hj = power(G.g, rep(r));
        ZZ_p htildej = power(gtilde, rep(r));
        ZZ_p::init(G.q);
        ZZ_p c = H_prime(G.g, gj, hj, gtilde, gtildej, htildej, G);
        ZZ_p z = r - (sj * c); //!!!
        proofj = ValidityProof{c, z}; //verification proof for index j
    }

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
        vec_ZZ_p indexes_A;
        for (int j = 0; j < n; j++){
            if (A.count(j) > 0){ // Party j is in A. Party j has the share f(j+1).
                indexes_A.append(ZZ_p(j+1));
            }
        }
        ZZ_p::init(G.q);
        vec_ZZ_p lagrange_coefs = getLagrange(G.q, indexes_A); 
        
        ZZ_p::init(G.p);
        vec_ZZ_p values_A;
        for (int j = 0; j < n; j++){
            if (A.count(j) > 0){ 
                values_A.append(coin_shares[j]);
            }
        }
        ZZ_p res = ZZ_p(1);
        // cout << "Thresh: " << indexes_A.length() << endl;
        // cout << "Thresh: " << lagrange_coefs << endl;
        // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for (long k = 0; k < indexes_A.length(); k++){
            res *= power(values_A[k], rep(lagrange_coefs[k]));
        }
        // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        // std::chrono::duration<double, std::milli> elapsed = end - begin;
        // cout << "Thresh: " << elapsed.count() << endl;
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
    void generateCoinShare(std::string C, long j, const ZZ_p& sj, const ZZ_p& Vj, ZZ_p& coinj, ValidityProof& proofj){
        ZZ_p::init(G.q);
        ZZ_p r = NTL::random_ZZ_p();
        ZZ_p::init(G.p);
        ZZ_p gtilde = H(C, G);
        ZZ_p gtildej = power(gtilde, rep(sj));
        coinj = gtildej; //coin share

        // ZZ_p gj = power(G.g, rep(sj));
        ZZ_p gj = Vj;
        ZZ_p hj = power(G.g, rep(r));
        ZZ_p htildej = power(gtilde, rep(r));
        ZZ_p::init(G.q);
        ZZ_p c = H_prime(G.g, gj, hj, gtilde, gtildej, htildej, G);
        ZZ_p z = r - (sj * c); //!!!
        proofj = ValidityProof{c, z}; //proof
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

        //combine shares
        // cout << "Gen: " << coin_shares_A.length() << endl;
        // cout << "Gen: " << lambda_A << endl;
        // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
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
        // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        // std::chrono::duration<double, std::milli> elapsed = end - begin;
        // cout << "Gen: " << elapsed.count() << endl;
        return res;
    }
}


int benchThreshold(PrimeOrderModGroup G, long t, long n, string marker, int repetitions = 1){
    std::vector<std::chrono::duration<double, std::milli>> elapsedKeyGen;
    std::vector<std::chrono::duration<double, std::milli>> elapsedGenerateCoin;
    std::vector<std::chrono::duration<double, std::milli>> elapsedVerify;
    std::vector<std::chrono::duration<double, std::milli>> elapsedCombine;
    std::chrono::duration<double, std::milli> elapsedKeyGenAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedGenerateCoinAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerifyAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedCombineAvg = std::chrono::milliseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::steady_clock::time_point begin2;
    std::chrono::steady_clock::time_point end2;
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
        vec_ZZ_p V; //verification keys
        vec_ZZ_p coin_shares;
        coin_shares.SetLength(ThresholdCoin::n);

        //generate keys        
        begin = std::chrono::steady_clock::now();
        ThresholdCoin::keyGen(s, V0, V, secret);
        end = std::chrono::steady_clock::now();
        elapsedKeyGen.push_back(end-begin);
        elapsedKeyGenAvg += end - begin;

        //generate coin shares
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < ThresholdCoin::n; j++){
            begin2 = std::chrono::steady_clock::now();
            ThresholdCoin::generateCoinShare(C, j, s[j], V[j], coin_shares[j], validity_proofs[j]);
            end2 = std::chrono::steady_clock::now();
            elapsedGenerateCoin.push_back(end2-begin2);
        }
        end = std::chrono::steady_clock::now();
        elapsedGenerateCoinAvg += (end - begin) / ThresholdCoin::n; //average time it takes a party to verify one share

        //verify
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < ThresholdCoin::n; j++){
            begin2 = std::chrono::steady_clock::now();
            if (!ThresholdCoin::verify(C, j, V[j], coin_shares[j], validity_proofs[j])){
                cout << "Problem: " << j << endl;
                return 1;
            }
            end2 = std::chrono::steady_clock::now();
            elapsedVerify.push_back(end2-begin2);
        }
        end = std::chrono::steady_clock::now();
        elapsedVerifyAvg += (end - begin) / ThresholdCoin::n; //average time it takes a party to verify one share

        //combine
        //Tested on theshold AS: getLagrange is slower than getRecombination, by ~0.15 ms for n=52, t=27
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
        elapsedCombine.push_back(end-begin);
        elapsedCombineAvg += end - begin;
        if (coin_value != power(gtilde, rep(secret))){
            cout << "Problem in combine: " << coin_value << endl;
            return 1;
        }
    }
    for (std::chrono::duration<double, std::milli> s : elapsedKeyGen){
        std::cout << marker << "." << n << ".KEYGEN. " << s.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> s : elapsedGenerateCoin){
        std::cout << marker << "." << n << ".GENER. " << s.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedVerify){
        std::cout << marker << "." << n << ".VERIF. " << c.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedCombine){
        std::cout << marker << "." << n << ".COMB. " << c.count() << endl;    
    }

    cout << marker << "." << n << ".KEYGENAVG. " << elapsedKeyGenAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".GENERAVG. " << elapsedGenerateCoinAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".VERIFAVG. " << elapsedVerifyAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".COMBAVG. " << elapsedCombineAvg.count() / repetitions << endl;
    return 0;
}


int benchGeneralized(PrimeOrderModGroup G, const string& quorumsConfFile, long n, string marker, int repetitions = 1){    
    std::vector<std::chrono::duration<double, std::milli>> elapsedKeyGen;
    std::vector<std::chrono::duration<double, std::milli>> elapsedGenerateCoin;
    std::vector<std::chrono::duration<double, std::milli>> elapsedVerify;
    std::vector<std::chrono::duration<double, std::milli>> elapsedCombine;
    std::vector<long> authorizedGroupSize;
    long authorizedGroupSizeAvg = 0;
    std::chrono::duration<double, std::milli> elapsedKeyGenAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedGenerateCoinAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerifyAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedCombineAvg = std::chrono::milliseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::steady_clock::time_point begin2;
    std::chrono::steady_clock::time_point end2;
    
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
        vec_ZZ_p V; //verification keys
        vec_ZZ_p coin_shares;
        coin_shares.SetLength(GeneralizedCoin::msp.m());

        //generate keys        
        begin = std::chrono::steady_clock::now();
        GeneralizedCoin::keyGen(s, V0, V, secret);
        end = std::chrono::steady_clock::now();
        elapsedKeyGen.push_back(end-begin);
        elapsedKeyGenAvg += end - begin;

        //generate coin shares
        std::map<int, std::chrono::duration<double, std::milli>> elapsedGeneratePerParty;
        std::map<int, int> sharesPerParty;
        for (long ii = 0; ii < n; ii++){
            elapsedGeneratePerParty[ii] = std::chrono::milliseconds(0);
            sharesPerParty[ii] = 0;
        }
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < GeneralizedCoin::msp.m(); j++){
            begin2 = std::chrono::steady_clock::now();    
            GeneralizedCoin::generateCoinShare(C, j, s[j], V[j], coin_shares[j], validity_proofs[j]);
            end2 = std::chrono::steady_clock::now();
            elapsedGeneratePerParty[GeneralizedCoin::msp.L[j]] += end2-begin2;
            sharesPerParty[GeneralizedCoin::msp.L[j]]++;
        }
        end = std::chrono::steady_clock::now();
        elapsedGenerateCoinAvg += (end - begin) / GeneralizedCoin::msp.m(); //average time it takes a party to generate one share, average over all shares of all parties
        for (long ii = 0; ii < n; ii++){
            elapsedGenerateCoin.push_back(elapsedGeneratePerParty[ii] / sharesPerParty[ii]); //average time it takes a party to generate one share, average over the shares of that party
        }

        //verify
        std::map<int, std::chrono::duration<double, std::milli>> elapsedVerifyPerParty;
        for (long ii = 0; ii < n; ii++){
            elapsedVerifyPerParty[ii] = std::chrono::milliseconds(0);
        }
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < GeneralizedCoin::msp.m(); j++){
            begin2 = std::chrono::steady_clock::now();
            if (!GeneralizedCoin::verify(C, j, V[j], coin_shares[j], validity_proofs[j])){
                cout << "Problem: " << j << endl;
                return 1;
            }
            end2 = std::chrono::steady_clock::now();
            elapsedVerifyPerParty[GeneralizedCoin::msp.L[j]] += end2-begin2;
            // sharesPerParty[GeneralizedCoin::msp.L[j]]++;
            // sharesPerParty allready calculated during generate() phase
        }
        end = std::chrono::steady_clock::now();
        elapsedVerifyAvg += (end - begin) / GeneralizedCoin::msp.m(); //average time it takes a party to verify one share, over all shares and all parties
        for (long ii = 0; ii < n; ii++){
            elapsedVerify.push_back(elapsedVerifyPerParty[ii] / sharesPerParty[ii]); //average time it takes a party to verify all of its shares, average over all shares of that party
        }

        //combine
        //Tested: If A_size > than the threshold, then some coefficients are small and the recombination is faster!
       std::unordered_set<hotstuff::ReplicaID> A = getAuthorizedSet(n, GeneralizedCoin::msp);
        // cout << "size " << A.size() << endl;
        
        ZZ_p::init(G.p);
        begin = std::chrono::steady_clock::now();
        ZZ_p coin_value = GeneralizedCoin::combine(A, coin_shares);
        end = std::chrono::steady_clock::now();
        elapsedCombine.push_back(end-begin);
        authorizedGroupSize.push_back(A.size());
        elapsedCombineAvg += end - begin;
        authorizedGroupSizeAvg += A.size();
        if (coin_value != power(gtilde, rep(secret))){
            cout << "Problem in combine: " << coin_value << endl;
            return 1;
        }

    }

    for (std::chrono::duration<double, std::milli> s : elapsedKeyGen){
        std::cout << marker << "." << n << ".KEYGEN. " << s.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> s : elapsedGenerateCoin){
        std::cout << marker << "." << n << ".GENER. " << s.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedVerify){
        std::cout << marker << "." << n << ".VERIF. " << c.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedCombine){
        std::cout << marker << "." << n << ".COMB. " << c.count() << endl;    
    }
    for (long s : authorizedGroupSize){
        std::cout << marker << "." << n << ".AUTH_SIZE. " << s << endl;    
    }

    cout << marker << "." << n << ".KEYGENAVG. " << elapsedKeyGenAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".GENERAVG. " << elapsedGenerateCoinAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".VERIFAVG. " << elapsedVerifyAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".COMBAVG. " << elapsedCombineAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".AUTH_SIZE_AVG. " << authorizedGroupSizeAvg / repetitions << endl;
    return 0;
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
    
    // int from = 3;
    // int to = 103;
    // int step = 10;
    // int repetitions = 1;
    // for (int n = from; n <= to; n+=step){
    //     benchThreshold(G, n / 2 + 1, n, "THR", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     std::string quorumSpec = "conf/quorum_confs/quorums_maj_thres_" + std::to_string(n) + ".json";
    //     benchGeneralized(G, quorumSpec, n, "GEN", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     benchThreshold(G, 2*n/3 + 1, n, "THRCOMPL", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     std::string quorumSpec = "conf/quorum_confs/quorums_thres_" + std::to_string(n) + ".json";
    //     benchGeneralized(G, quorumSpec, n, "GENCOMPL", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     std::string quorumSpec = "conf/quorum_confs/unbalanced_" + std::to_string(n) + ".json";
    //     benchGeneralized(G, quorumSpec, n, "UNBAL", repetitions);
    // }

    int repetitions = 10;
    for (auto n: {4, 16, 36, 64, 100}){
        benchThreshold(G, n / 2 + 1, n, "THR", repetitions);
        std::string quorumSpec = "conf/quorum_confs/quorums_maj_thres_" + std::to_string(n) + ".json";
        benchGeneralized(G, quorumSpec, n, "GEN_MAJ", repetitions);
        quorumSpec = "conf/quorum_confs/unbalanced_" + std::to_string(n) + ".json";
        benchGeneralized(G, quorumSpec, n, "GEN_UNBAL", repetitions);
        quorumSpec = "conf/quorum_confs/kgrid_" + std::to_string(n) + ".json";
        benchGeneralized(G, quorumSpec, n, "GEN_KGRID", repetitions);
    }    
   return 0;
}
