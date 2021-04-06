#ifndef _QUORUMS_H
#define _QUORUMS_H  

#include <vector>
#include <string>
#include <unordered_set> 
#include <iostream>
#include <sstream>   
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/matrix.h>

#include "hotstuff/type.h"
#include "hotstuff/linalg_util.h"


namespace hotstuff::quorums{

struct Theta {
    int threshold;
    std::vector<hotstuff::ReplicaID> elements;
    std::vector<Theta> nestedElements;

    Theta(int threshold, 
          std::vector<hotstuff::ReplicaID> elements,
          std::vector<Theta> nestedElements): 
        threshold(threshold), elements(elements), nestedElements(nestedElements){}
    
    Theta(): threshold(0){}

    // Returns (an approximation of) the size of an instance of type Theta.
    std::size_t size(){
        size_t size = sizeof(Theta);
        size += elements.size() * sizeof(hotstuff::ReplicaID);
        for (auto nestedElem : nestedElements){
            size += nestedElem.size();
        }
        return size;
    }
};

std::ostream& operator<<(std::ostream& os, const Theta& t);
bool operator==(const Theta& lhs, const Theta& rhs);

class JsonParser{
    public:
    virtual hotstuff::quorums::Theta parse(const std::string& conf = "") = 0;
};

class DefaultConfigJsonParser: public JsonParser{
    public:
    hotstuff::quorums::Theta parse(const std::string& conf = "");
};   

class ConfigJsonParser: public JsonParser{
    public:
    hotstuff::quorums::Theta parse(const std::string& conf);
};   

class StringJsonParser: public JsonParser{
    public:
    hotstuff::quorums::Theta parse(const std::string& conf = "");
};

class Msp{
    public: 
    NTL::mat_ZZ_p M; 
    // Matrix M of msp

    std::vector<hotstuff::ReplicaID> L; 
    // Surjective labelling function rho of msp
    
    int m() const{return M.NumRows();};

    int d() const {return M.NumCols();}

    NTL::vec_ZZ_p e1() const{
        NTL::vec_ZZ_p e1;
        e1.SetLength(d());
        e1[0] = 1;
        return e1;
    }
    
    Msp(){}
    
    Msp(NTL::mat_ZZ_p M, std::vector<hotstuff::ReplicaID> L): M(M), L(L){}

    NTL::mat_ZZ_p U; //The U matrix from PLU factorization, such that P M^T = L U
    
    NTL::vec_ZZ_p y; //The solution of L y = P e1. 
    //Use this in equation Ua x = y. See method isAuthorizedGroupWithPLU

    bool isAuthorizedGroupWithoutPLU(std::unordered_set<ReplicaID> reps) const;
    bool isAuthorizedGroupWithPLU(std::unordered_set<ReplicaID> reps) const;
    
    bool isAuthorizedGroup(std::unordered_set<ReplicaID> reps) const{
        return isAuthorizedGroupWithPLU(reps);
    }

    NTL::mat_ZZ_p getRowsOfMOwnedByReps(std::unordered_set<ReplicaID> reps) const;
    NTL::mat_ZZ_p getColsOfUOwnedByReps(std::unordered_set<ReplicaID> reps) const;

    NTL::mat_ZZ_p getAugmentedMatrix(NTL::mat_ZZ_p& augmMatrix, const NTL::mat_ZZ_p& coefMatrix, const NTL::vec_ZZ_p& constMatrix) const;

    operator std::string() const {
        std::string s;
        s.append("Matrix M of msp is:\n"); 
        std::stringstream buffer;
        buffer << M;
        s.append(buffer.str());
        s.append("\nMatrix L of msp is:\n"); 
        for (int i = 0; i < L.size(); i++) { 
            s.append(std::to_string(i)).append("->").append(std::to_string(L[i])).append(" "); 
        }
        s.append("\n");
        return s;
    }

    //Returns (an approximation of) the size  in Bytes of an instance of type Msp.
    std::size_t size(){
        size_t size = sizeof(Msp);
        size += M.NumCols() * M.NumRows() * sizeof(NTL::ZZ_p);
        size += L.size() * sizeof(hotstuff::ReplicaID);
        size += U.NumCols() * U.NumRows() * sizeof(NTL::ZZ_p);
        size += y.length() * sizeof(NTL::ZZ_p);
        return size;
    }

    // Used by the dealer in BLS-HotStuff with generalized trust.
    // Returns the secret, the shares and the corresp. owners.
    void shareRandSecret(NTL::ZZ_p &secret, NTL::vec_ZZ_p &shares, std::vector<hotstuff::ReplicaID> &owners);
    vec_ZZ_p getRecombinationVector(std::unordered_set<ReplicaID> reps) const;

};


class MspCreator{
    public:
    virtual hotstuff::quorums::Msp create(const hotstuff::quorums::Theta& t) = 0;
};

class InsertionMspCreator: public MspCreator{
    public:
    NTL::mat_ZZ_p getVandermonde(const Theta &t);

    NTL::mat_ZZ_p getVandermonde(long threshold, long numPoints);

    NTL::mat_ZZ_p  performInsertion(const NTL::mat_ZZ_p &M1, const NTL::mat_ZZ_p &M2, const long &r);
    //insertion of matrix M2 (2D, described by t) in the row of matrix M (2D) specified by the index r, with r in [0, m1-1]

    void insertNextTheta(NTL::mat_ZZ_p &M, std::vector<hotstuff::ReplicaID> &L, int &insertionIndex, const Theta &t);
    //insertion of access structure t in the row of matrix M (2D) specified by the index r, with r in [0, m1-1]

    hotstuff::quorums::Msp createMspWithInsertionAlgorithm(const hotstuff::quorums::Theta& t);

    void performPLU(hotstuff::quorums::Msp& msp);
    //Updates msp.U and msp.y with the results of PLU (P M^T = L U) and of the equation L y = msp.e1

    hotstuff::quorums::Msp create(const hotstuff::quorums::Theta& t){
        Msp msp = createMspWithInsertionAlgorithm(t);
        performPLU(msp);
        return msp;
    }
};


class AccessStructure{
    hotstuff::quorums::JsonParser *jsonParser = new DefaultConfigJsonParser(); 
    hotstuff::quorums::MspCreator *mspCreator = new InsertionMspCreator(); 
    
    public:
    hotstuff::quorums::Theta thetaOperatorFormula;
    hotstuff::quorums::Msp msp;

    AccessStructure(){};
    AccessStructure(hotstuff::quorums::JsonParser *jsonParser_): AccessStructure() {jsonParser = jsonParser_;}

    void initialize(const std::string& conf = ""){
        thetaOperatorFormula = jsonParser->parse(conf);    
        msp = mspCreator->create(thetaOperatorFormula);      
    }

    bool isAuthorizedGroup(std::unordered_set<ReplicaID> reps) const{
#ifdef GENERALIZED_QUORUMS_WITH_MSP
    return msp.isAuthorizedGroup(reps);
#else
    return evalFomula(reps);
#endif  
    };

    bool evalFomula(const std::unordered_set<ReplicaID> reps) const{
        return evalFomula(thetaOperatorFormula, reps);
    }
    bool evalFomula(const hotstuff::quorums::Theta formula, const std::unordered_set<ReplicaID> reps) const;

    operator std::string() const {
#ifdef GENERALIZED_QUORUMS_WITH_MSP
         return std::string(msp);
#else
        std::stringstream buffer;
        buffer << thetaOperatorFormula;
        return buffer.str();
#endif
    }
};

}
#endif