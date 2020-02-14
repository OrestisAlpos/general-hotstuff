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


struct Msp{
    NTL::mat_ZZ_p M; // Matrix M of msp
    std::vector<hotstuff::ReplicaID> L; // Surjective labelling function rho of msp

    int m; //set to M.NumRows()
    int d; //set to M.NumCols()
    NTL::vec_ZZ_p e1;//Set to target vector (1, 0, 0 ,...) of dimension d

    operator std::string () const {
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

    bool isAuthorisedGroup(std::unordered_set<ReplicaID> reps) const;
    NTL::mat_ZZ_p getRowsOwnedByGroup(std::unordered_set<ReplicaID> reps) const;
    NTL::mat_ZZ_p getAugmentedMatrix(NTL::mat_ZZ_p coefMatrix, NTL::vec_ZZ_p constMatrix) const;
};

class AccessStructureParser{
    const NTL::ZZ p = NTL::ZZ(101);

    Msp getLcwMsp(const Theta &t);
    void insertNextTheta(std::vector<std::vector<int> > &M, std::vector<hotstuff::ReplicaID> &L, int &insertionIndex, const Theta &t);
    void performInsertion(std::vector< std::vector<int> > &M1, std::vector< std::vector<int> > M2, const int r);
    std::vector< std::vector<int> > getVandermonde(const Theta &t);

    public:
    AccessStructureParser(){
        NTL::ZZ_p::init(p);
    }

    Msp parse();
    
};

}
#endif