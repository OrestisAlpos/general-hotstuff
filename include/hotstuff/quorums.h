#ifndef _QUORUMS_H
#define _QUORUMS_H

#include <vector>
#include <string> 
#include <iostream>

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
    std::vector< std::vector<int> > M; // Matrix M of msp
    std::vector<hotstuff::ReplicaID> L; // Surjective labelling function rho of msp

    operator std::string () const {
        std::string s;
        s.append("Matrix M of msp is:\n"); 
	    for (int i = 0; i < M.size(); i++) { 
            for (int j = 0; j < M[i].size(); j++) 
                s.append(std::to_string(M[i][j])).append("  "); 
            s.append("\n"); 
        }
        s.append("Matrix L of msp is:\n"); 
        for (int i = 0; i < L.size(); i++) { 
            s.append(std::to_string(i)).append("->").append(std::to_string(L[i])).append(" "); 
        }
        return s;
    }
};

class AccessStructureParser{
    Msp getLcwMsp(const Theta &t);
    void insertNextTheta(Msp &msp, int &insertionIndex, const Theta &t);
    void performInsertion(std::vector< std::vector<int> > &M1, std::vector< std::vector<int> > M2, const int r);
    std::vector< std::vector<int> > getVandermonde(const Theta &t);

    public:
    Msp parse();
};

}
#endif