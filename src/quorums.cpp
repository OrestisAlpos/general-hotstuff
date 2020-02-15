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


//Json Parser class implemementation
hotstuff::quorums::Theta hotstuff::quorums::JsonParser::parse_IdBased(){
   std::ifstream infile("quorums.json");
    json j;
    infile >> j;
    return j.get<Theta>();

}

hotstuff::quorums::Theta hotstuff::quorums::JsonParser::parse_IdBased(const std::string& conf){
    json j = json::parse(conf);
    return j.get<Theta>();
}


//MspCreator class implementation
std::vector< std::vector<int> > hotstuff::quorums::MspCreator::getVandermonde(const Theta &t){
    return getVandermonde(t.threshold, t.elements.size() + t.nestedElements.size());
}

std::vector< std::vector<int> > hotstuff::quorums::MspCreator::getVandermonde(int threshold, int numPoints){
    std::vector< std::vector<int> > v;
    for (int i = 1; i <= numPoints; i++) {
        std::vector<int> row;
        for (int power = 0; power < threshold; power++)
            row.push_back(pow(i, power));
        v.push_back(row);  
    }
    return v; 
}

void hotstuff::quorums::MspCreator::performInsertion(
        std::vector< std::vector<int> > &M1, // m1 x d1
        std::vector< std::vector<int> > M2,  // m2 x d2
        const int r){
    int m1 = M1.size();
    int d1 = M1[0].size();
    int m2 = M2.size();
    int d2 = M2[0].size();
    for (int i = 0; i < r ; i++)
    {
        for (int j = 1; j < d2; j++)
        {
            M1[i].push_back(0);
        }
    }
    for (int i = r + 1; i < m1; i++)    
    {
        for (int j = 1; j < d2; j++)
        {
            M1[i].push_back(0);
        }
    }
    std::vector rowToRepeat = M1[r];
    for (int j2 = 1; j2 < d2; j2++)
        {
            M1[r].push_back(M2[0][j2]);
        }
    for (int i2 = 1; i2 < m2; i2++)
    {
        std::vector<int> newRow = rowToRepeat;
        for (int j2 = 1; j2 < d2; j2++)
        {
            newRow.push_back(M2[i2][j2]);
        }
        M1.insert(M1.begin() + r + i2, newRow);
    }
    
}

void hotstuff::quorums::MspCreator::insertNextTheta(std::vector<std::vector<int> > &M, 
                                                               std::vector<hotstuff::ReplicaID> &L,
                                                               int &insertionIndex, 
                                                               const Theta &t){
    std::vector< std::vector<int> > M2 = getVandermonde(t);
    performInsertion(M, M2, insertionIndex);
    for (auto it = t.elements.begin(); it != t.elements.end(); ++it){
        L.push_back(*it);
    }
    insertionIndex += t.elements.size();
    for (auto it = t.nestedElements.begin(); it != t.nestedElements.end(); ++it) {
        insertNextTheta(M, L, insertionIndex, (*it));
    }
}

Msp hotstuff::quorums::MspCreator::createWithLcwAlgorithm(const Theta &t){
    std::vector<std::vector<int> > M = {{1}};
    std::vector<hotstuff::ReplicaID> L;
    int insertionIndex = 0;
    insertNextTheta(M, L, insertionIndex, t);
    Msp msp;
    msp.M.SetDims(M.size(), M[0].size());
    for (auto i = 0; i < M.size(); i++)
        for (auto j = 0; j < M[0].size(); j++)
            msp.M[i][j] = ZZ_p(M[i][j]);
    msp.L = L;
    return msp;
}

bool hotstuff::quorums::Msp::isAuthorisedGroup(std::unordered_set<ReplicaID> reps) const{
    mat_ZZ_p Ma = getRowsOwnedByGroup(reps);
    mat_ZZ_p Ma_T = NTL::transpose(Ma);
    mat_ZZ_p Ma_T_augm = getAugmentedMatrix(Ma_T, e1());
    int deg_Ma_T = NTL::gauss(Ma_T);
    int deg_Ma_T_augm = NTL::gauss(Ma_T_augm);
    return deg_Ma_T == deg_Ma_T_augm;
}

NTL::mat_ZZ_p hotstuff::quorums::Msp::getAugmentedMatrix(NTL::mat_ZZ_p coefMatrix, NTL::vec_ZZ_p constMatrix) const{
    mat_ZZ_p augmMatrix;
    assert(coefMatrix.NumRows() == constMatrix.length());
    augmMatrix.SetDims(coefMatrix.NumRows(), coefMatrix.NumCols() + 1);
    for (int i = 0; i < augmMatrix.NumRows(); i++){
        for (int j = 0; j < augmMatrix.NumCols() - 1; j++){
            augmMatrix[i][j] = coefMatrix[i][j];
        }
        augmMatrix[i][augmMatrix.NumCols() - 1] = constMatrix[i];
    }
    return augmMatrix;
}

NTL::mat_ZZ_p hotstuff::quorums::Msp::getRowsOwnedByGroup(std::unordered_set<ReplicaID> reps) const{
    mat_ZZ_p Ma;
    Ma.SetDims(0, d());
    for (int i = 0; i < m(); i++){
        if (reps.count(L[i]) > 0){
            Ma.SetDims(Ma.NumRows() + 1, d());
            for (int j = 0; j < d(); j++)
                Ma[Ma.NumRows() - 1][j] = M[i][j];
        }
    }
    return Ma;
}

}