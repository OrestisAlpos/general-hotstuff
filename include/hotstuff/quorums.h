#ifndef _QUORUMS_H
#define _QUORUMS_H

#if !defined(QUORUMS_PRIME_P)
#define QUORUMS_PRIME_P 101
#endif

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
};
std::ostream& operator<<(std::ostream& os, const Theta& t);
bool operator==(const Theta& lhs, const Theta& rhs);


class JsonParser{
    public:
    hotstuff::quorums::Theta parse_IdBased();

    hotstuff::quorums::Theta parse_IdBased(const std::string& conf);

    hotstuff::quorums::Theta parse(){ return parse_IdBased();   }
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
    
    NTL::mat_ZZ_p U; //The U matrix from PLU factorization, such that P M^T = L U
    
    NTL::vec_ZZ_p y; //The solution of L y = P e1. 
    //Use this in equation Ua x = y. See method isAuthorizedGroupWithPLU

    bool isAuthorisedGroupWithoutPLU(std::unordered_set<ReplicaID> reps) const;
    bool isAuthorisedGroupWithPLU(std::unordered_set<ReplicaID> reps) const;
    
    bool isAuthorisedGroup(std::unordered_set<ReplicaID> reps) const{
        return isAuthorisedGroupWithPLU(reps);
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
};


class MspCreator{
    public:
    std::vector< std::vector<int> > getVandermonde(const Theta &t);

    std::vector< std::vector<int> > getVandermonde(int threshold, int numPoints);

    void performInsertion(std::vector< std::vector<int> > &M1, std::vector< std::vector<int> > M2, const int r);
    //insertion of matrix M2 (2D, described by t) in the row of matrix M (2D) specified by the index r, with r in [0, m1-1]

    void insertNextTheta(std::vector<std::vector<int> > &M, std::vector<hotstuff::ReplicaID> &L, int &insertionIndex, const Theta &t);
    //insertion of access structure t in the row of matrix M (2D) specified by the index r, with r in [0, m1-1]

    hotstuff::quorums::Msp createMspWithLcwAlgorithm(const hotstuff::quorums::Theta& t);

    void performPLU(hotstuff::quorums::Msp& msp);
    //Updates msp.U and msp.y with the results of PLU (P M^T = L U) and of the equation L y = msp.e1

    hotstuff::quorums::Msp create(const hotstuff::quorums::Theta& t){
        Msp msp = createMspWithLcwAlgorithm(t);
        performPLU(msp);
        return msp;
    }
};


class AccessStructure{
    hotstuff::quorums::JsonParser jsonParser; 
    hotstuff::quorums::MspCreator mspCreator; 
    hotstuff::quorums::Msp msp;

    public:
    AccessStructure(){
        NTL::ZZ_p::init(NTL::ZZ(QUORUMS_PRIME_P));
    };

    void initialize(){
        Theta t = jsonParser.parse();
        msp = mspCreator.create(t); 
        
    }

    bool isAuthorisedGroup(std::unordered_set<ReplicaID> reps) const{
        return msp.isAuthorisedGroup(reps);
    };

    operator std::string() const { return std::string(msp);}
};


}
#endif