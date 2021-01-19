#include "hotstuff/quorums.h"
#include "hotstuff/linalg_util.h"
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/matrix.h>
#include <numeric>
#include <algorithm>
#include <random> 
#include <chrono> 
#include <stdlib.h>

using namespace std;
using namespace hotstuff::quorums;
using namespace NTL;

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

int main(){
    hotstuff::quorums::AccessStructure as;
    hotstuff::quorums::JsonParser *jp = new hotstuff::quorums::ConfigJsonParser();
    as = hotstuff::quorums::AccessStructure(jp);

    std::vector<int> numRepl{4, 7, 13, 19, 25, 31};
    std::vector<int> kvals{4, 6, 8, 10};
    //for(int n : numRepl){
    for(int k : kvals){    
        int n = 4 * k;
        //as.initialize("conf/quorum_confs/quorums_thres_" + to_string(n) + ".json");
        as.initialize("conf/quorum_confs/1common_k" + to_string(k) + ".json");
        std::cout << "FORM." + to_string(n) + ".mem. " << as.thetaOperatorFormula.size() << endl; //Memory in Bytes
        std::cout << "ON." + to_string(n) + ".mem. " << as.msp.size() << endl;
        std::cout << "OFF." + to_string(n) + ".mem. " << 2 * sizeof(n) << endl;
        // std::vector<hotstuff::ReplicaID> allReplicas(n);
        // std::iota(allReplicas.begin(), allReplicas.end(), 1);
        // unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        // srand(seed);
        // std::unordered_set<hotstuff::ReplicaID> setToCheck;
        std::chrono::duration<double, std::micro> elapsedFormula = std::chrono::microseconds(0);
        std::chrono::duration<double, std::micro> elapsedMsp = std::chrono::microseconds(0);
        std::chrono::duration<double, std::micro> elapsedCounting = std::chrono::microseconds(0);
        std::chrono::steady_clock::time_point begin;
        std::chrono::steady_clock::time_point end;
        int repetitions = 10000;
        for (int i = 0; i < repetitions; i++)
        {
            std::unordered_set<hotstuff::ReplicaID> randomSet = getRandomSet(n);
            //FORMULA
            begin = std::chrono::steady_clock::now();
            as.evalFomula(randomSet);
            end = std::chrono::steady_clock::now();
            elapsedFormula += end - begin;
            //MSP
            begin = std::chrono::steady_clock::now();
            as.msp.isAuthorizedGroup(randomSet);
            end = std::chrono::steady_clock::now();
            elapsedMsp += end - begin;
            //COUNTING
            begin = std::chrono::steady_clock::now();
            randomSet.size() > 2 * n / 3;
            end = std::chrono::steady_clock::now();
            elapsedCounting += end - begin;
            //std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
        }
        
        std::cout << "FORM." + to_string(n) + ".time. " << elapsedFormula.count() / repetitions << std::endl; // time in microseconds
        // std::cout << "Time difference Formula = " << std::chrono::duration_cast<std::chrono::microseconds>(elapsedFormula).count() / repetitions << "[µs]" << std::endl;
        std::cout << "ON." + to_string(n) + ".time. " << elapsedMsp.count() / repetitions << std::endl;
        std::cout << "OFF." + to_string(n) + ".time. " << elapsedCounting.count() / repetitions << std::endl;
    }
}