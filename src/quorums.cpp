#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <math.h>

#include "hotstuff/quorums.h"
#include "hotstuff/util.h"

using json = nlohmann::json;

namespace hotstuff::quorums {

void to_json(json& j, const Theta& t) {
    throw std::runtime_error("");
}

void from_json(const json& j, Theta& t) {
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
 
std::vector< std::vector<int> > hotstuff::quorums::AccessStructureParser::getVandermonde(const Theta &t){
    std::vector< std::vector<int> > v;
    int numPoints = t.elements.size() + t.nestedElements.size();
    for (int i = 1; i <= numPoints; i++) {
        std::vector<int> row;
        for (int power = 0; power < t.threshold; power++)
            row.push_back(pow(i, power));
        v.push_back(row);  
    }
    return v;    
}

//insertion of matrix M2 (2D, described by t) in the row of matrix M (2D) specified by the index r, with r in [0, m1-1]
void hotstuff::quorums::AccessStructureParser::performInsertion(
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

//insertion of access structure t in the row of matrix M (2D) specified by the index r, with r in [0, m1-1]
void hotstuff::quorums::AccessStructureParser::insertNextTheta(Msp &msp, int &insertionIndex, const Theta &t){
    std::vector< std::vector<int> > M2 = getVandermonde(t);
    performInsertion(msp.M, M2, insertionIndex);
    for (auto it = t.elements.begin(); it != t.elements.end(); ++it){
        msp.L.push_back(*it);
    }
    insertionIndex += t.elements.size();
    for (auto it = t.nestedElements.begin(); it != t.nestedElements.end(); ++it) {
        insertNextTheta(msp, insertionIndex, (*it));
    }
}

Msp hotstuff::quorums::AccessStructureParser::getLcwMsp(const Theta &t){
    Msp msp;
    msp.M = {{1}}; 
    int insertionIndex = 0;
    insertNextTheta(msp, insertionIndex, t);
    return msp;
}

Msp hotstuff::quorums::AccessStructureParser::parse(){
    std::ifstream infile("quorums.json");
    json j;
    infile >> j;
    Theta t;
    j.get_to<Theta>(t);
    return getLcwMsp(t);
}

}




// int main(int argc, char const *argv[]) {
//     // testThetaStruct();
    
//     std::ifstream infile("quorums.json");
//     json j;
//     infile >> j;
//     hotstuff::Theta p2;
//     j.get_to<hotstuff::Theta>(p2);
//     std::cout << p2 << std::endl;
    
//     // printVect(getVandermonde(3,3));
//     // printVect(getVandermonde(4,3));
//     // printVect(getVandermonde(4,4));
//     // printVect(getVandermonde(7,5));  

//     // hotstuff::Msp msp = getLcwMsp(p2);
//     // printVect(msp.M);
//     // for (auto const& pair: hotstuff::allReplicas) {
//     //     std::cout << "{" << pair.first << ": " << pair.second << "}\n";
//     // }
//     // printVect(msp.L);
//     return  0;
// }



// void testThetaStruct(){
//     // auto j = json::parse(" { \"select\": 3,\"out-of\": [ {\"select\": 1, \"out-of\":[2,3]}, {\"select\": 1, \"out-of\":[4,5]}, 1]}");
//     std::cout << ">> THETA TEST START\n";
//     std::vector<hotstuff::ReplicaID> e1 = {1, 2, 3, 4};
//     std::vector<hotstuff::ReplicaID> e2 = {11, 12, 13, 14};
//     std::vector<hotstuff::ReplicaID> e3 = {21, 22, 23, 24};
//     hotstuff::Theta theta1 {3, e1, {}};
//     hotstuff::Theta theta2 {2, e2, {}};
//     hotstuff::Theta theta3(4, e3, std::vector<hotstuff::Theta>{theta1, theta2});
//     std::cout << theta3;
//     std::cout << "\n>> THETA TEST FINISH\n";
// }