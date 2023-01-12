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
#include <cmath>

#include <iostream>
#include <fstream>

using namespace NTL;
using namespace std;

std::unordered_set<hotstuff::ReplicaID> getRandomSet(const int n){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    int randomBits = std::rand();
    int randomBitsUsed = 0;
    int randomBitsAvailable = 8 * sizeof(RAND_MAX);
    std::unordered_set<hotstuff::ReplicaID> randomSet;
    for (int i = 0; i < n; i++)
    {
        bool b = randomBits & 1;
        randomBits = randomBits >> 1;
        randomBitsUsed++;
        if (randomBitsUsed == randomBitsAvailable){
            randomBits = std::rand();
            randomBitsUsed = 0;
        }
        if (b  == 1) randomSet.insert(hotstuff::ReplicaID(i));
    }
    return randomSet;
}

void benchAuthorizedGroupSize(PrimeOrderModGroup G, const string& quorumsConfFile, long n, string marker, int repetitions, bool onlyAvg = false){
    ZZ_p::init(G.q);
    hotstuff::quorums::JsonParser *jp = new hotstuff::quorums::ConfigJsonParser();
    hotstuff::quorums::AccessStructure accessStructure(jp);
    accessStructure.initialize(quorumsConfFile);
    hotstuff::quorums::Msp msp = accessStructure.msp;
    
    //Write MSP to a separate file
    ofstream o;
    std::string filename = "plots/msp_" + marker + "_" + std::to_string(n);
    o.open(filename);
    o << "### " << marker << " " << std::to_string(n) << " ###" << endl;
    o << std::string(msp);
    o.close();

    //Calculate size of the MSP
    cout << marker << "." << n << ".MSP_SIZE. " << msp.size() << endl;

    std::vector<long> authorizedGroupSize; //size of set until it becomes authorized
    long authorizedGroupSizeAvg = 0;
    std::vector<long> actualAuthorizedGroupSize; //size of actual authorized
    long actualAuthorizedGroupSizeAvg = 0;
    long smallCoefficients = 0;
    std::vector<long> totalBits;
    long totalBitsAvg = 0;

    for (int i = 0; i < repetitions; i++){
        std::unordered_set<hotstuff::ReplicaID> A = getAuthorizedSet(n, msp);
        authorizedGroupSize.push_back(A.size());
        authorizedGroupSizeAvg += A.size();
        vec_ZZ_p lambda_A = msp.getRecombinationVector(A);
        long actualAuthSize = 0;
        long bitsInRecombinationVector = 0;
        for (ZZ_p coef: lambda_A){
            // ZZ_p coef = lambda_A[l];
            if (! iszero(coef)){
                actualAuthSize++;
                bitsInRecombinationVector += ceil(log(coef._ZZ_p__rep)/ 0.693147); //Base change, to log-base-2
                if (coef._ZZ_p__rep < ZZ(9999) ) {
                   smallCoefficients++;
                }
            }
        }
        actualAuthorizedGroupSize.push_back(actualAuthSize);
        actualAuthorizedGroupSizeAvg += actualAuthSize;
        totalBits.push_back(bitsInRecombinationVector);
        totalBitsAvg += bitsInRecombinationVector; 
    }

    if (! onlyAvg){
        for (long s : authorizedGroupSize){
            std::cout << marker << "." << n << ".AUTH_SIZE. " << s << endl;    
        }
        for (long s : actualAuthorizedGroupSize){
            std::cout << marker << "." << n << ".ACTUAL_MSP_ROWS. " << s << endl;    
        }
         for (long s : totalBits){
            std::cout << marker << "." << n << ".TOTAL_BITS. " << s << endl;    
        }
    }
    cout << marker << "." << n << ".AUTH_SIZE_AVG. " << authorizedGroupSizeAvg / repetitions << endl;
    cout << marker << "." << n << ".ACTUAL_MSP_ROWS_AVG. " << actualAuthorizedGroupSizeAvg / repetitions << endl;
    cout << marker << "." << n << ".SMALL_COEFFS. " << smallCoefficients / repetitions << endl;
    cout << marker << "." << n << ".TOTAL_BITS_AVG. " << totalBitsAvg / repetitions << endl;
}

void benchMbfSizeAndTime(const string& quorumsConfFile, long n, string marker, int repetitions, bool onlyAvg = false){
    hotstuff::quorums::AccessStructure as;
    hotstuff::quorums::JsonParser *jp = new hotstuff::quorums::ConfigJsonParser();
    as = hotstuff::quorums::AccessStructure(jp);
    as.initialize(quorumsConfFile);

    double mbfSize = as.thetaOperatorFormula.size() * 0.001; // Memory in KB
    cout << marker << "." << n << ".mem. " << mbfSize << endl;
    
    std::chrono::duration<double, std::micro> elapsed = std::chrono::microseconds(0);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    for (int i = 0; i < repetitions; i++)
    {
        std::unordered_set<hotstuff::ReplicaID> randomSet = getRandomSet(n);
        //FORMULA
        begin = std::chrono::steady_clock::now();
        as.evalFomula(randomSet);
        end = std::chrono::steady_clock::now();
        elapsed = end - begin;
    
        cout << marker << "." << n << ".time. " << elapsed.count() << std::endl; // time in microseconds
    }
}

int main(){    
    // ZZ p = ZZ(23);
    // ZZ q = ZZ(11);
    ZZ p,q;
    long p_size = 3072; //3072
    long q_size = 256;  //256
    // generatePrimes(p_size, q_size, p, q);
    PrimeOrderModGroup G = getGroup(p_size,q_size);
    
    int repetitions = 1000;
    for (auto n: {4, 16, 36, 64, 100}){
        std::string quorumSpec = "conf/quorum_confs/quorums_maj_thres_" + std::to_string(n) + ".json";
        // benchAuthorizedGroupSize(G, quorumSpec, n, "GEN_MAJ", repetitions, false);
        benchMbfSizeAndTime(quorumSpec, n, "GEN_MAJ", repetitions, false);
        quorumSpec = "conf/quorum_confs/unbalanced_" + std::to_string(n) + ".json";
        // benchAuthorizedGroupSize(G, quorumSpec, n, "GEN_UNBAL", repetitions, false);
        benchMbfSizeAndTime(quorumSpec, n, "GEN_UNBAL", repetitions, false);
        quorumSpec = "conf/quorum_confs/kgrid_" + std::to_string(n) + ".json";
        // benchAuthorizedGroupSize(G, quorumSpec, n, "GEN_KGRID", repetitions, false);
        benchMbfSizeAndTime(quorumSpec, n, "GEN_KGRID", repetitions, false);
    }
    ZZ_p x = ZZ_p(8);
   return 0;
}