#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <math.h>

#include "hotstuff/quorums.h"
#include "hotstuff/util.h"

using json = nlohmann::json;
using namespace NTL;

namespace hotstuff::quorums {

//Operators and functions overload for type hotstuff::quorums::Theta
std::ostream& operator<<(std::ostream& os, const Theta& t){   
    os << "[";
    os << "Threshold: " << t.threshold << " Elements: ";
    for (auto it = t.elements.begin(); it != t.elements.end(); ++it)
        os << *it << ", "; 
    if (t.nestedElements.size() > 0){
        for (auto it = t.nestedElements.begin(); it != t.nestedElements.end(); ++it)
            os << *it << ", ";
    }
    os << "]";    
    return os;
};

bool operator==(const Theta& lhs, const Theta& rhs){
    return (lhs.threshold == rhs.threshold &&
            lhs.elements == rhs.elements &&
            lhs.nestedElements == rhs.nestedElements);
};

void to_json(json& j, const Theta& t){
    throw std::runtime_error("");
}

void from_json(const json& j, Theta& t){
    j.at("select").get_to(t.threshold);
    auto j2 = j.at("out-of");
    for (json::iterator it = j2.begin(); it != j2.end(); ++it) {
        try {
            (*it).at("select"); //Check if (*it) is a nested Theta. If no exception, then it is.
            Theta nestedElement;
            (*it).get_to<Theta>(nestedElement);
            t.nestedElements.push_back(nestedElement);
        }
        catch(const json::type_error& e) {
            hotstuff::ReplicaID rid = (*it);
            t.elements.push_back(rid);
        }  
    }
}


//JsonParser subclasses implemementation
hotstuff::quorums::Theta hotstuff::quorums::DefaultConfigJsonParser::parse(const std::string& conf){
    std::ifstream infile("quorums.json");
    json j;
    infile >> j;
    return j.get<Theta>();
}

hotstuff::quorums::Theta hotstuff::quorums::ConfigJsonParser::parse(const std::string& conf){
    std::ifstream infile(conf);
    json j;
    infile >> j;
    return j.get<Theta>();
}

hotstuff::quorums::Theta hotstuff::quorums::StringJsonParser::parse(const std::string& conf /* = "" */){
    json j = json::parse(conf);
    return j.get<Theta>();
}


//MspCreator subclasses implementation
NTL::mat_ZZ_p hotstuff::quorums::InsertionMspCreator::getVandermonde(const Theta &t){
    return getVandermonde(t.threshold, t.elements.size() + t.nestedElements.size());
}

NTL::mat_ZZ_p hotstuff::quorums::InsertionMspCreator::getVandermonde(long threshold, long numPoints){
    mat_ZZ_p v;
    ZZ_p p;
    v.SetDims(numPoints, threshold);
    for (long i = 1; i <= numPoints; i++) {
        for (long j = 0; j < threshold; j++){
            NTL::power(p, ZZ_p(i), ZZ(j));
            conv(v[i-1][j], p);
        }
    }
    return v;
}

NTL::mat_ZZ_p hotstuff::quorums::InsertionMspCreator::performInsertion(
                                            const NTL::mat_ZZ_p &M1, // m1 x d1
                                            const NTL::mat_ZZ_p &M2,  // m2 x d2
                                            const long &r){ // M1(r -> M2)
    long m1 = M1.NumRows();
    long d1 = M1.NumCols();
    long m2 = M2.NumRows();
    long d2 = M2.NumCols();
    NTL::mat_ZZ_p M;
    M.SetDims(m1 + m2 - 1, d1 + d2 - 1);
    //rows 0 to r-1
    for (long i = 0; i <= r - 1 ; i++)
        //copy values from corresponding M1 cols
        for (long j = 0; j <= d1 - 1; j++)
            M[i][j] = M1[i][j];
    //rows r to r+m2-1
    for (long i = r; i <= r + m2 - 1 ; i++){
        //copy values from r-1 row of M1
        for (long j = 0; j <= d1 - 1; j++)
            M[i][j] = M1[r][j];
        //copy the rest cols from M2
        for (long j = d1; j <= d1 + d2 - 2; j++)
            M[i][j] = M2[i - r][j - d1 + 1];
    }
    //rows r+m2 to m1+m2-2
    for (int i = r + m2; i <= m1 + m2 - 2; i++)
        //copy values from corresponding M1 cols
        for (int j = 0; j <= d1 - 1; j++)
            M[i][j] = M1[i - m2 + 1][j];
    return M;
}

void hotstuff::quorums::InsertionMspCreator::insertNextTheta( 
                                    NTL::mat_ZZ_p &M, 
                                    std::vector<hotstuff::ReplicaID> &L,
                                    int &insertionIndex, 
                                    const Theta &t){
    NTL::mat_ZZ_p M2 = getVandermonde(t);
    M = performInsertion(M, M2, insertionIndex);
    for (auto it = t.elements.begin(); it != t.elements.end(); ++it){
        L.push_back(*it);
    }
    insertionIndex += t.elements.size();
    for (auto it = t.nestedElements.begin(); it != t.nestedElements.end(); ++it) {
        insertNextTheta(M, L, insertionIndex, (*it));
    }
}

Msp hotstuff::quorums::InsertionMspCreator::createMspWithInsertionAlgorithm(const Theta &t){
    NTL::mat_ZZ_p M;
    M.SetDims(1,1);
    M[0][0] = ZZ_p(1);
    std::vector<hotstuff::ReplicaID> L;
    int insertionIndex = 0;
    insertNextTheta(M, L, insertionIndex, t);
    return Msp(M, L);
}

void hotstuff::quorums::InsertionMspCreator::performPLU(hotstuff::quorums::Msp& msp){
    NTL::mat_ZZ_p P, L;
    linalg::PLU(NTL::transpose(msp.M), P, L, msp.U);
    linalg::solveByForwardSubstitution(L, msp.y, P * msp.e1());
}


//Msp class implementation
bool hotstuff::quorums::Msp::isAuthorizedGroupWithoutPLU(std::unordered_set<ReplicaID> reps) const{
    mat_ZZ_p Ma = getRowsOfMOwnedByReps(reps);
    mat_ZZ_p Ma_T;
    NTL::transpose(Ma_T, Ma);
    mat_ZZ_p Ma_T_augm;
    getAugmentedMatrix(Ma_T_augm, Ma_T, e1());
    int rank_Ma_T = NTL::gauss(Ma_T);
    int rank_Ma_T_augm = NTL::gauss(Ma_T_augm);
    //system Ma^T x = e1 has solution iff rank of Ma^T is same as rank of Ma^T|e1
    return rank_Ma_T == rank_Ma_T_augm;
}

bool hotstuff::quorums::Msp::isAuthorizedGroupWithPLU(std::unordered_set<ReplicaID> reps) const{
    mat_ZZ_p Ua = getColsOfUOwnedByReps(reps);
    mat_ZZ_p Ua_augm;
    getAugmentedMatrix(Ua_augm, Ua, y);
    int rank_Ua = linalg::gauss(Ua); //Ua is almost-upper-triangular. linalg::gauss checks for a 0 below the pivot.
    int rank_Ua_augm = linalg::gauss(Ua_augm);
    //system Ua x = y has solution iff rank of Ua is same as rank of Ua|y
    return rank_Ua == rank_Ua_augm;
}

NTL::mat_ZZ_p hotstuff::quorums::Msp::getAugmentedMatrix(NTL::mat_ZZ_p& augmMatrix, 
                                                         const NTL::mat_ZZ_p& coefMatrix,
                                                         const NTL::vec_ZZ_p& constMatrix) const{
    assert(coefMatrix.NumRows() == constMatrix.length());
    augmMatrix.kill();
    augmMatrix.SetDims(coefMatrix.NumRows(), coefMatrix.NumCols() + 1);
    for (long i = 0; i < augmMatrix.NumRows(); i++){
        for (long j = 0; j < augmMatrix.NumCols() - 1; j++){
            augmMatrix[i][j] = coefMatrix[i][j];
        }
        augmMatrix[i][augmMatrix.NumCols() - 1] = constMatrix[i];
    }
    return augmMatrix;
}

NTL::mat_ZZ_p hotstuff::quorums::Msp::getRowsOfMOwnedByReps(std::unordered_set<ReplicaID> reps) const{
    long Ma_NumRows = 0;
    for (long i = 0; i < m(); i++)
        if (reps.count(L[i]) > 0)
            Ma_NumRows ++;
    mat_ZZ_p Ma;
    Ma.SetDims(Ma_NumRows, d());
    long row_index = 0;
    for (long i = 0; i < m(); i++){
        if (reps.count(L[i]) > 0){
            for (long j = 0; j < d(); j++)
                Ma[row_index][j] = M[i][j];
            row_index++;
        }
    }
    return Ma;
}

NTL::mat_ZZ_p hotstuff::quorums::Msp::getColsOfUOwnedByReps(std::unordered_set<ReplicaID> reps) const{
    long Ua_NumCols = 0;
      for (long j = 0; j < m(); j++)
        if (reps.count(L[j]) > 0)
            Ua_NumCols++; 
    mat_ZZ_p Ua;
    Ua.SetDims(d(), Ua_NumCols);
    long last_col = 0;
    for (long j = 0; j < m(); j++){
        if (reps.count(L[j]) > 0){
            for (long i = 0; i < d(); i++)
                Ua[i][last_col] = U[i][j];
            last_col++;
        }
    }
    return Ua;
}


//AccessStructure class implementation
bool hotstuff::quorums::AccessStructure::evalFomula(const hotstuff::quorums::Theta formula, const std::unordered_set<ReplicaID> reps) const{
    int true_vals = 0;
    for (auto const& elem: formula.elements){
        if (reps.count(elem) > 0) true_vals++;
    }
    for (auto const& nested_theta: formula.nestedElements){
        if (evalFomula(nested_theta, reps)) true_vals++;
    }
    return true_vals >= formula.threshold;
}
}
