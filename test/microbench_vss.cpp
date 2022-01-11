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


namespace ThresholdVss{
    long t,n;
    PrimeOrderModGroup G;

    void init(const PrimeOrderModGroup& group, const long& threshold, const long& n_parties){
        G = group;
        t = threshold;
        n = n_parties;
    }

    //share a secret in Zq
    void share(ZZ_p secret, vec_ZZ_p& s, vec_ZZ_p& s_prime, vec_ZZ_p& C){
        ZZ_p::init(G.q);
        ZZ_pX f;
        while (true){
            f = random_ZZ_pX(t); // f(x) = a_0 + a_1 * x + a_2 * x^2 + ... + a_{t-1} * x^{t-1}, where a_0 = secret
            if (deg(f) == t-1) break;
        }
        SetCoeff(f, 0, secret);
        // cout << "Polynomial f: " << f << endl;
        ZZ_pX f_prime;
        while (true){
            f_prime = random_ZZ_pX(t);
            if (deg(f_prime) == t-1) break;
        }
        // cout << "Polynomial f': " << f_prime << endl;
        clear(s); s.SetLength(n); 
        clear(s_prime); s_prime.SetLength(n);
        for (long j = 0; j < n; j++){ //party 0 gets f(1), ..., party i gets f(i+1)
            ZZ_p point = ZZ_p(j+1);
            s[j] = eval(f, point);
            s_prime[j] = eval(f_prime, point);
        }
        ZZ_p::init(G.p);
        clear(C); C.SetLength(t);
        for (long l = 0; l < t; l++){
            C[l] = power(G.g, rep(coeff(f, l))) * power(G.h, rep(coeff(f_prime, l))); // C1 = g^s * h^s' \in G, Cl = g^{a_l} * h^{a'_l} \in G
        }    
    }

    bool verify(long j, ZZ_p sj, ZZ_p sj_prime, vec_ZZ_p C){ 
        ZZ_p::init(G.q);
        ZZ_p point = ZZ_p(j+1); //Party j has share at point j+1, i.e., f(j+1)
        ZZ_p::init(G.p);
        ZZ_p part1 = power(G.g, rep(sj)) * power(G.h, rep(sj_prime));
        ZZ_p part2 = ZZ_p(1); // Prod{l=0...t-1} Cl^{x^l} = {Cl^x}^l, where x is the point (j+1) for party j
        vec_ZZ exponents; exponents.SetLength(t); // Cache computations of x^l to avoid frequent modulo switching
        ZZ_p::init(G.q);
        for (long l = 0; l < t; l++){            
            exponents[l] = rep(power(point, l));
        }
        ZZ_p::init(G.p);
        for (long l = 0; l < t; l++){
            part2 *= power(C[l], exponents[l]);
        }
        // cout << "Verification finished :" << j << endl;
        return part1 == part2;
    }

    ZZ_p reconstruct(const std::unordered_set<hotstuff::ReplicaID>& A, const vec_ZZ_p& s){ //For testing only, pass all shares and use only those owned by parties in A
        ZZ_p::init(G.q);
        vec_ZZ_p indexes_A;
        vec_ZZ_p values_A;
        for (int j = 0; j < n; j++){
            if (A.count(j) > 0){ // Party j is in A. Party j has the share f(j+1).
                indexes_A.append(ZZ_p(j+1));
                values_A.append(s[j]);
            }
        }
        ZZ_pX f_interp = interpolate(indexes_A, values_A);
        // cout << "Reconstruction finished " << endl;
        return eval(f_interp, ZZ_p(0));
    }
}

namespace GeneralizedVss{
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

    //share a secret in Zq
    void share(ZZ_p secret, vec_ZZ_p& s, vec_ZZ_p& s_prime, vec_ZZ_p& C){
        ZZ_p::init(G.q);
        vec_ZZ_p r = random_vec_ZZ_p(msp.d());
        r[0] = secret;
        vec_ZZ_p r_prime = random_vec_ZZ_p(msp.d());
        s = msp.M * r;
        s_prime = msp.M * r_prime;
        ZZ_p::init(G.p);
        clear(C); C.SetLength(msp.d());
        for (long l = 0; l < msp.d(); l++){
            C[l] = power(G.g, rep(r[l])) * power(G.h, rep(r_prime[l])); // C1 = g^s * h^s' \in G, Cl = g^{r_l} * h^{r'_l} \in G
        }
        // cout << "Sharing finished :" << endl;    
    }

    bool verify(long j, ZZ_p sj, ZZ_p sj_prime, vec_ZZ_p C){ // The row-index j has shares sj, sj_prime
        ZZ_p::init(G.p);
        ZZ_p part1 = power(G.g, rep(sj)) * power(G.h, rep(sj_prime));
        ZZ_p part2 = ZZ_p(1);
        for (long l = 0; l < msp.d(); l++){
            part2 *= power(C[l], rep(msp.M[j][l]));
        }
        // cout << "Verification finished :" << j << endl;
        return part1 == part2;
    }

    ZZ_p reconstruct(const std::unordered_set<hotstuff::ReplicaID>& A, const vec_ZZ_p& s){ //For testing only, pass all shares and use only those owned by parties in A
        ZZ_p::init(G.q);
        vec_ZZ_p s_A;
        for (int j = 0; j < s.length(); j++){
            if (A.count(msp.L[j]) > 0){
                s_A.append(s[j]);
            }
        }
        vec_ZZ_p lambda_A = msp.getRecombinationVector(A);
        return lambda_A * s_A;
    }
}


int benchThreshold(PrimeOrderModGroup G, long t, long n, string marker, int repetitions = 1){
    std::vector<std::chrono::duration<double, std::milli>> elapsedShare;
    std::vector<std::chrono::duration<double, std::milli>> elapsedVerify;
    std::vector<std::chrono::duration<double, std::milli>> elapsedReconstruct;
    std::chrono::duration<double, std::milli> elapsedShareAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerifyAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedReconstructAvg = std::chrono::milliseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::steady_clock::time_point begin2;
    std::chrono::steady_clock::time_point end2;

    for (int i = 0; i < repetitions; i++){
        ZZ_p::init(G.q);
        ZZ_p secret = random_ZZ_p();

        //init
        ThresholdVss::init(G, t, n);

        //share
        ZZ_p::init(G.q);
        vec_ZZ_p s; vec_ZZ_p s_prime;
        ZZ_p::init(G.p);
        vec_ZZ_p C;
        begin = std::chrono::steady_clock::now();
        ThresholdVss::share(secret, s, s_prime, C);
        end = std::chrono::steady_clock::now();
        elapsedShare.push_back(end-begin);
        elapsedShareAvg += end - begin; //time to share a secret

        //verify
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < ThresholdVss::n; j++){
            begin2 = std::chrono::steady_clock::now();
            if (!ThresholdVss::verify(j, s[j], s_prime[j], C)){
                cout << "Problem in share verification: " << j << endl;
                return 1;
            }
            end2 = std::chrono::steady_clock::now();
            elapsedVerify.push_back(end2-begin2);
        }
        end = std::chrono::steady_clock::now();
        elapsedVerifyAvg += (end - begin) / ThresholdVss::n; //average time it takes a party to verify one share

        //reconsruct
        std::vector<hotstuff::ReplicaID> S;
        for (long i = 0; i < ThresholdVss::n; i++){
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

        ZZ_p::init(G.q);
        begin = std::chrono::steady_clock::now();
        ZZ_p secret_recomb = ThresholdVss::reconstruct(A, s);
        end = std::chrono::steady_clock::now();
        elapsedReconstruct.push_back(end-begin);
        elapsedReconstructAvg += end - begin;
        if (secret_recomb != secret){
            cout << "Problem in reconstruction: " << secret_recomb << endl;
            return 1;
        }
    }

    for (std::chrono::duration<double, std::milli> s : elapsedShare){
        std::cout << marker << "." << n << ".SHARE. " << s.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedVerify){
        std::cout << marker << "." << n << ".VERIF. " << c.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedReconstruct){
        std::cout << marker << "." << n << ".RECON. " << c.count() << endl;    
    }

    cout << marker << "." << n << ".SHAREAVG. " << elapsedShareAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".VERIFAVG. " << elapsedVerifyAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".RECONAVG. " << elapsedReconstructAvg.count() / repetitions << endl;
}


int benchGeneralized(PrimeOrderModGroup G, const string& quorumsConfFile, long n, string marker, int repetitions = 1){    
    std::vector<std::chrono::duration<double, std::milli>> elapsedShare;
    std::vector<std::chrono::duration<double, std::milli>> elapsedVerify;
    std::vector<std::chrono::duration<double, std::milli>> elapsedReconstruct;
    std::chrono::duration<double, std::milli> elapsedShareAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerifyAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedReconstructAvg = std::chrono::milliseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::steady_clock::time_point begin2;
    std::chrono::steady_clock::time_point end2;
    
    for (int i = 0; i < repetitions; i++){
        ZZ_p::init(G.q);
        ZZ_p secret = random_ZZ_p();

        //init
        GeneralizedVss::init(G, quorumsConfFile);

        //share
        ZZ_p::init(G.q);
        vec_ZZ_p s; vec_ZZ_p s_prime;
        ZZ_p::init(G.p);
        vec_ZZ_p C;
        begin = std::chrono::steady_clock::now();
        GeneralizedVss::share(secret, s, s_prime, C);
        end = std::chrono::steady_clock::now();
        elapsedShare.push_back(end-begin);
        elapsedShareAvg += end - begin; //time to share a secret

        //verify
        std::map<int, std::chrono::duration<double, std::milli>> elapsedPerParty;
        for (long ii = 0; ii < n; ii++){
            elapsedPerParty[ii] = std::chrono::milliseconds(0);
        }
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < GeneralizedVss::msp.m(); j++){ 
            begin2 = std::chrono::steady_clock::now();
            if (!GeneralizedVss::verify(j, s[j], s_prime[j], C)){
                cout << "Problem in generalized share verification: " << j << endl;
                return 1;
            }
            end2 = std::chrono::steady_clock::now();
            elapsedPerParty[GeneralizedVss::msp.L[j]] += end2-begin2;
        }
        end = std::chrono::steady_clock::now();
        elapsedVerifyAvg += (end - begin) / GeneralizedVss::msp.m(); //average time it takes a party to verify one individual share
        for (long ii = 0; ii < n; ii++){
            elapsedVerify.push_back(elapsedPerParty[ii]); //average time it takes a party to verify all of its shares
        }
        //reconsruct
        std::vector<hotstuff::ReplicaID> S;
        for (long i = 0; i < n; i++){
            S.push_back(i);
        }
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        auto gen = std::default_random_engine(seed);
        shuffle(S.begin(), S.end(), gen);
        std::unordered_set<hotstuff::ReplicaID> A;
        while (true){
            long A_size;
            if (marker == "GEN" || marker == "GENCOMPL"){
                A_size = GeneralizedVss::msp.d(); //CHANGE THIS IF YOU USE GENERALIZED ACCESS STRUCTURE
            }else{
                A_size = rand()%n + 1;
            }
            for (long j = 0; j < A_size; j++){
                A.insert(S.at(j));
            }
            if (GeneralizedVss::msp.isAuthorizedGroup(A)){
                break;
            }
        }
        // cout << "size " << A.size() << endl;

        ZZ_p::init(G.q);
        begin = std::chrono::steady_clock::now();
        ZZ_p secret_recomb = GeneralizedVss::reconstruct(A, s);
        end = std::chrono::steady_clock::now();
        elapsedReconstruct.push_back(end-begin);
        elapsedReconstructAvg += end - begin;
        if (secret_recomb != secret){
            cout << "Problem in generalized reconstruction: " << secret_recomb << endl;
            return 1;
        }

    }

    for (std::chrono::duration<double, std::milli> s : elapsedShare){
        std::cout << marker << "." << n << ".SHARE. " << s.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedVerify){
        std::cout << marker << "." << n << ".VERIF. " << c.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedReconstruct){
        std::cout << marker << "." << n << ".RECON. " << c.count() << endl;    
    }

    cout << marker << "." << n << ".SHAREAVG. " << elapsedShareAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".VERIFAVG. " << elapsedVerifyAvg.count() / repetitions << endl;
    cout << marker << "." << n << ".RECONAVG. " << elapsedReconstructAvg.count() / repetitions << endl;
}


int main(){    
    // ZZ p = ZZ(23);
    // ZZ q = ZZ(11);

    ZZ p,q;
    long p_size = 3072; //3072
    long q_size = 256;  //256
    // generatePrimes(p_size, q_size, p, q);
    PrimeOrderModGroup G = getGroup(p_size,q_size);
    
    // //GENERALIZED
    // GeneralizedVss::init(G, "conf/quorum_confs/quorums_thres_4.json");
    
    // //share
    // ZZ_p::init(G.q);
    // // cout << std::string(GeneralizedVss::msp) << endl;
    // vec_ZZ_p s; vec_ZZ_p s_prime; vec_ZZ_p C;
    // ZZ_p secret = ZZ_p(7);
    // GeneralizedVss::share(secret, s, s_prime, C);
    // // cout << "s: " << s << endl << "s': " << s_prime << endl << "C: " << C << endl;
    
    // //verify
    // for (long j = 0; j < GeneralizedVss::msp.m(); j++){
    //     if (!GeneralizedVss::verify(j, s[j], s_prime[j], C)){
    //         cout << "Problem: " << j << endl;
    //         return 1;
    //     }  
    // }
    
    // //reconstruct
    // ZZ_p::init(G.q);
    // ZZ_p secret_recomb = GeneralizedVss::reconstruct({0,1,2}, s);
    // if (secret_recomb != secret){
    //     cout << "Problem in reconstruction: " << secret_recomb << endl;
    //     return 1;
    // }
    
    // //THRESHOLD
    // long t = 3;
    // long n = 4;
    // ThresholdVss::init(G, t, n);

    // //share
    // ZZ_p::init(G.q);
    // secret = ZZ_p(7);
    // ThresholdVss::share(secret, s, s_prime, C);
    // // cout << "s: " << s << endl << "s': " << s_prime << endl << "C: " << C << endl;
    
    // //verify
    // for (long j = 0; j < ThresholdVss::n; j++){
    //     if (!ThresholdVss::verify(j, s[j], s_prime[j], C)){
    //         cout << "Problem: " << j << endl;
    //         return 1;
    //     }
    // }
    
    // //reconstruct
    // ZZ_p::init(G.q);
    // secret_recomb = ThresholdVss::reconstruct({0,1,2}, s);
    // if (secret_recomb != secret){
    //     cout << "Problem in reconstruction: " << secret_recomb << endl;
    //     return 1;
    // }
   

    // for (int n = 5; n < 30; n+=2){
    //     benchThreshold(G, n);
    // }
    int from = 3;
    int to = 103;
    int step = 10;
    int repetitions = 200;
    for (int n = from; n <= to; n+=step){
        benchThreshold(G, n / 2 + 1, n, "THR", repetitions);
    }
    for (int n = from; n <= to; n+=step){
        std::string quorumSpec = "conf/quorum_confs/quorums_maj_thres_" + std::to_string(n) + ".json";
        benchGeneralized(G, quorumSpec, n, "GEN", repetitions);
    }
    // for (int n = from; n <= to; n+=step){
    //     benchThreshold(G, 2*n/3 + 1, n, "THRCOMPL", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     std::string quorumSpec = "conf/quorum_confs/quorums_thres_" + std::to_string(n) + ".json";
    //     benchGeneralized(G, quorumSpec, n, "GENCOMPL", repetitions);
    // }
    for (int n = from; n <= to; n+=step){
        std::string quorumSpec = "conf/quorum_confs/unbalanced_" + std::to_string(n) + ".json";
        benchGeneralized(G, quorumSpec, n, "UNBAL", repetitions);
    }
   return 0;
}