#include <random> 
#include <chrono> 
#include <stdlib.h>
#include <map>
#include "hotstuff/crypto.h"
#include "hotstuff/quorums.h"
#include "algebra_utils.h"

using namespace std;

inline bls::Id Fr_ntl_to_mcl_Id(const NTL::ZZ_p val){
    std::stringstream buffer;
    buffer << val;
    bls::Id val_mcl;
    val_mcl.setStr(buffer.str(), 10);
    return val_mcl;
}

inline bls::SecretKey Fr_ntl_to_mcl_Sec(const NTL::ZZ_p val){
    std::stringstream buffer;
    buffer << val;
    bls::SecretKey val_mcl;
    val_mcl.setStr(buffer.str(), 10);
    return val_mcl;
}

int benchThreshold( long t, long n, string marker, int repetitions = 1){
    std::vector<std::chrono::duration<double, std::milli>> elapsedSign;
    std::vector<std::chrono::duration<double, std::milli>> elapsedVerify;
    std::vector<std::chrono::duration<double, std::milli>> elapsedCombine;
    std::chrono::duration<double, std::milli> elapsedSignAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerifyAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedCombineAvg = std::chrono::milliseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::steady_clock::time_point begin2;
    std::chrono::steady_clock::time_point end2;

    for (int i = 0; i < repetitions; i++){
        std::string msg = "message" + std::to_string(i);

        //Get keys
        bls::SecretKey sec;
        bls::PublicKey pub;
        sec.init(); //generates random sk
        
        sec.getPublicKey(pub);
        bls::SecretKeyVec msk;			//msk contains the t coefs. of the polynomial
        sec.getMasterSecretKey(msk, t); //makes msk[0]==sec and the other entries random
        bls::SecretKeyVec secVec(n);
        bls::IdVec idVec(n);
        bls::PublicKeyVec pubVec(n);

        for (size_t j = 0; j < n; j++) {
            int id = j + 1;          //Replicas are numbered 0..n-1, but they get the values at points 1..n of the sharing polynomial.
            idVec[j] = id;
            secVec[j].set(msk, id); //evaluates the polynomial of degree msk.size()-1 with coefs. msk on point id, id>0
            secVec[j].getPublicKey(pubVec[j]);
        }
        
        //SIGN with BLS
        bls::SignatureVec sigVec(n);
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < n; j++){
            begin2 = std::chrono::steady_clock::now();
            secVec[j].sign(sigVec[j], msg);
            end2 = std::chrono::steady_clock::now();
            elapsedSign.push_back(end2-begin2);
        }
        end = std::chrono::steady_clock::now();
        elapsedSignAvg += (end - begin) / n;
        
        //VERIFY with BLS
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < n; j++){
            begin2 = std::chrono::steady_clock::now();
            if (!(sigVec[j].verify(pubVec[j], msg))){
                cout << "Problem: " << j << endl;
                return 1;
            }
            end2 = std::chrono::steady_clock::now();
            elapsedVerify.push_back(end2-begin2);
        }    
        end = std::chrono::steady_clock::now();
        elapsedVerifyAvg += (end - begin) / n;

        //COMBINE partial signatures
        std::vector<hotstuff::ReplicaID> S;
        for (long j = 0; j < n; j++){
            S.push_back(j);
        }
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        auto gen = std::default_random_engine(seed);
        shuffle(S.begin(), S.end(), gen);
        std::unordered_set<hotstuff::ReplicaID> A;
        bls::SignatureVec sigsToCombine(t);
        bls::IdVec idsToCombine(t);
        vec_ZZ_p indexes_A;
        for (long j = 0; j < t; j++){
            long id = S.at(j);
            A.insert(S.at(j));
            idsToCombine[j] = idVec[S.at(j)];
            sigsToCombine[j] = sigVec[S.at(j)];
            indexes_A.append(ZZ_p(S.at(j) + 1));
        }
        // cout << "size " << A.size() << endl;

        std::string FrOrderStr;
        bls::getCurveOrder(FrOrderStr);

        begin = std::chrono::steady_clock::now();
        bls::Signature s;
        //Don't use s.recover from mcl. It is way slower than calculating first the Lagrange coefficients with my getLagrange() and then the aggr. signature
        // s.recover(sigsToCombine, idsToCombine);
        
        vec_ZZ_p lagrange_coefs_ntl = getLagrange(NTL::ZZ(NTL::INIT_VAL, FrOrderStr.c_str()), indexes_A);
        
        bls::IdVec lagrange_coefs(lagrange_coefs_ntl.length());
        for (long j = 0; j < lagrange_coefs_ntl.length(); j++)
            lagrange_coefs[j] = Fr_ntl_to_mcl_Id(lagrange_coefs_ntl[j]); 
        
        s.recoverGeneralised(sigsToCombine, lagrange_coefs);

        end = std::chrono::steady_clock::now();
        elapsedCombine.push_back(end-begin);
        elapsedCombineAvg += end - begin;
        
        if (!(s.verify(pub, msg))){
            cout << "Problem with combined signature " << endl;
                return 1;
        }
        bls::Signature s2;
        sec.sign(s2, msg);
        if (s != s2){
            cout << "Problem with combined signature " << endl;
                return 1;
        }
        
    }
    for (std::chrono::duration<double, std::milli> s : elapsedSign){
        std::cout << marker << "." << n << ".SIGN. " << s.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> v : elapsedVerify){
        std::cout << marker << "." << n << ".VERIF. " << v.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedCombine){
        std::cout << marker << "." << n << ".COMB. " << c.count() << endl;    
    }
    std::cout << marker << "." << n << ".SIGNAVG. " << elapsedSignAvg.count() / repetitions << endl;
    std::cout << marker << "." << n << ".VERIFAVG. " << elapsedVerifyAvg.count() / repetitions << endl;
    std::cout << marker << "." << n << ".COMBAVG. " << elapsedCombineAvg.count() / repetitions << endl;
    return 0;
}


int benchGeneralized(const string& quorumsConfFile, long n, string marker, int repetitions = 1){
    std::vector<std::chrono::duration<double, std::milli>> elapsedSign;
    std::vector<std::chrono::duration<double, std::milli>> elapsedVerify;
    std::vector<std::chrono::duration<double, std::milli>> elapsedCombine;
    std::chrono::duration<double, std::milli> elapsedSignAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedVerifyAvg = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> elapsedCombineAvg = std::chrono::milliseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::steady_clock::time_point begin2;
    std::chrono::steady_clock::time_point end2;

    for (int i = 0; i < repetitions; i++){
        std::string msg = "message" + std::to_string(i);

        //Get keys
        hotstuff::quorums::JsonParser *jp = new hotstuff::quorums::ConfigJsonParser();
        hotstuff::quorums::AccessStructure as(jp);
        as.initialize(quorumsConfFile);

        NTL::ZZ_p secret_ntl;
        NTL::vec_ZZ_p shares_ntl;
        std::vector<hotstuff::ReplicaID> owners;
        as.msp.shareRandSecret(secret_ntl, shares_ntl, owners);

        bls::SecretKey sec = Fr_ntl_to_mcl_Sec(secret_ntl);
        bls::PublicKey pub;
        sec.getPublicKey(pub);
        bls::SecretKeyVec secVec(as.msp.m());
        // bls::IdVec idVec(as.msp.m());
        bls::PublicKeyVec pubVec(as.msp.m());

        for (size_t j = 0; j < as.msp.m(); j++) {
            // idVec[j] = bls::Id(owners[j]);
            bls::SecretKey share_mcl = Fr_ntl_to_mcl_Sec(shares_ntl[j]);
            secVec[j] = share_mcl;
            secVec[j].getPublicKey(pubVec[j]);
        }
        
        //SIGN with BLS
        std::map<int, std::chrono::duration<double, std::milli>> elapsedSignPerParty;
        std::map<int, int> sharesPerParty;
        for (long ii = 0; ii < n; ii++){
            elapsedSignPerParty[ii] = std::chrono::milliseconds(0);
            sharesPerParty[ii] = 0;
        }
        bls::SignatureVec sigVec(as.msp.m());
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < as.msp.m(); j++){
            begin2 = std::chrono::steady_clock::now();
            secVec[j].sign(sigVec[j], msg);
            end2 = std::chrono::steady_clock::now();
            elapsedSignPerParty[as.msp.L[j]] += end2-begin2;
            sharesPerParty[as.msp.L[j]]++;
        }
        end = std::chrono::steady_clock::now();
        elapsedSignAvg += (end - begin) / as.msp.m(); //average time it takes a party to generate one signature share, average over all parties and all their shares
        for (long ii = 0; ii < n; ii++){
            elapsedSign.push_back(elapsedSignPerParty[ii] / sharesPerParty[ii]); //average time it takes a party to generate one of its signature shares
        }

        //VERIFY with BLS
        std::map<int, std::chrono::duration<double, std::milli>> elapsedVerifyPerParty;
        for (long ii = 0; ii < n; ii++){
            elapsedVerifyPerParty[ii] = std::chrono::milliseconds(0);
        }
        begin = std::chrono::steady_clock::now();
        for (long j = 0; j < as.msp.m(); j++){
            begin2 = std::chrono::steady_clock::now();
            if (!(sigVec[j].verify(pubVec[j], msg))){
                cout << "Problem: " << j << endl;
                return 1;
            }
            end2 = std::chrono::steady_clock::now();
            elapsedVerifyPerParty[as.msp.L[j]] += end2-begin2;
        }    
        end = std::chrono::steady_clock::now();
        elapsedVerifyAvg += (end - begin) / as.msp.m();  //average time it takes a party to verify one signature share
        for (long ii = 0; ii < n; ii++){
            elapsedVerify.push_back(elapsedVerifyPerParty[ii] / sharesPerParty[ii]); //average time it takes a party to verify one of its signature shares
        }

        //COMBINE partial signatures
        std::unordered_set<hotstuff::ReplicaID> A = getAuthorizedSet(n, as.msp);
        // cout << "size " << A.size() << endl;

        // vec_ZZ_p indexes_A;
        // std::string FrOrderStr;
        // bls::getCurveOrder(FrOrderStr);

        bls::SignatureVec sigsToCombine;
        for (int j = 0; j < as.msp.m(); j++){
            if (A.count(as.msp.L[j]) > 0){
                sigsToCombine.push_back(sigVec[j]);
                // idsToCombine.push_back(idVec[j]);
                // indexes_A.append(ZZ_p(as.msp.L[j]) + 1);
            }
        }
        
        begin = std::chrono::steady_clock::now();
        NTL::vec_ZZ_p recomb_vector_ntl = as.msp.getRecombinationVector(A);
        bls::IdVec recomb_vector_mcl(recomb_vector_ntl.length());
        for (long j = 0; j < recomb_vector_ntl.length(); j++)
            recomb_vector_mcl[j] = Fr_ntl_to_mcl_Id(recomb_vector_ntl[j]);
        
        bls::Signature s;
        // cout << "Gen uses: " << std::to_string(sigsToCombine.size()) << endl;
        // cout << recomb_vector_ntl << endl;
        s.recoverGeneralised(sigsToCombine, recomb_vector_mcl);
        end = std::chrono::steady_clock::now();
        elapsedCombine.push_back(end-begin);
        elapsedCombineAvg += end - begin;
        
        if (!(s.verify(pub, msg))){
            cout << "Problem with combined generalized signature " << endl;
                return 1;
        }
        bls::Signature s2;
        sec.sign(s2, msg);
        if (s != s2){
            cout << "Problem with combined generalized signature " << endl;
                return 1;
        }
        
    }
    for (std::chrono::duration<double, std::milli> s : elapsedSign){
        std::cout << marker << "." << n << ".SIGN. " << s.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> v : elapsedVerify){
        std::cout << marker << "." << n << ".VERIF. " << v.count() << endl;    
    }
    for (std::chrono::duration<double, std::milli> c : elapsedCombine){
        std::cout << marker << "." << n << ".COMB. " << c.count() << endl;    
    }
    std::cout << marker << "." << n << ".SIGNAVG. " << elapsedSignAvg.count() / repetitions << endl;
    std::cout << marker << "." << n << ".VERIFAVG. " << elapsedVerifyAvg.count() / repetitions << endl;
    std::cout << marker << "." << n << ".COMBAVG. " << elapsedCombineAvg.count() / repetitions << endl;
    return 0;
}


int main(){
    bls::init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
    std::string FrOrderStr;
    bls::getCurveOrder(FrOrderStr);
    NTL::ZZ_p::init(NTL::ZZ(NTL::INIT_VAL, FrOrderStr.c_str()));


    // int from = 3;
    // int to = 103;
    // int step = 20;
    // int repetitions = 10;
    // for (int n = from; n <= to; n+=step){
    //     benchThreshold(n / 2 + 1, n, "THR", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     std::string quorumSpec = "conf/quorum_confs/quorums_maj_thres_" + std::to_string(n) + ".json";
    //     benchGeneralized(quorumSpec, n, "GEN", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     benchThreshold(2*n/3 + 1, n, "THRCOMPL", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     std::string quorumSpec = "conf/quorum_confs/quorums_thres_" + std::to_string(n) + ".json";
    //     benchGeneralized(quorumSpec, n, "GENCOMPL", repetitions);
    // }
    // for (int n = from; n <= to; n+=step){
    //     std::string quorumSpec = "conf/quorum_confs/unbalanced_" + std::to_string(n) + ".json";
    //     benchGeneralized(quorumSpec, n, "UNBAL", repetitions);
    // }
    
    int repetitions = 10;
    for (auto n: {4, 16, 36, 64, 100}){
        benchThreshold(n / 2 + 1, n, "THR", repetitions);
        std::string quorumSpec = "conf/quorum_confs/quorums_maj_thres_" + std::to_string(n) + ".json";
        benchGeneralized(quorumSpec, n, "GEN_MAJ", repetitions);
        quorumSpec = "conf/quorum_confs/unbalanced_" + std::to_string(n) + ".json";
        benchGeneralized(quorumSpec, n, "GEN_UNBAL", repetitions);
        quorumSpec = "conf/quorum_confs/kgrid_" + std::to_string(n) + ".json";
        benchGeneralized(quorumSpec, n, "GEN_KGRID", repetitions);
    }    

    return 0;
}