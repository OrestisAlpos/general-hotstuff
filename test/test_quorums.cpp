#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "hotstuff/quorums.h"
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/matrix.h>

using namespace std;
using namespace hotstuff::quorums;
using namespace NTL;

Theta LcwExample1(){
    Theta theta1 = {2, {1, 2, 3}, {}};
    Theta theta2 = {2, {4, 5, 6}, {}};
    Theta theta3 = {3, {9, 10, 11, 12}, {}};
    Theta theta4 = {2, {7, 8}, {theta3}};
    return {2, {}, {theta1, theta2, theta4}};
}

Theta SimpleThreshold1(){
    return {2, {1, 2, 3, 4}, {}};
}

TEST_CASE( ) {
    ZZ_p::init(ZZ(QUORUMS_PRIME_P));
    hotstuff::quorums::JsonParser jsonParser;
    hotstuff::quorums::MspCreator mspCreator;

     SECTION("JsonParser", "Test the methods for parsing a Json string to a Theta-based structure" ) {    
        Theta theta_parsed = jsonParser.parse_IdBased(
            " { \"select\": 2, "
            "   \"out-of\": [ {\"select\": 2, \"out-of\":[1, 2, 3]} , "
                             "{\"select\": 2, \"out-of\":[4, 5, 6]} , "
                             "{\"select\": 2, \"out-of\":[7, 8, {\"select\": 3, \"out-of\":[9, 10, 11, 12] } ]} "
                           "] }"
        );
    
        Theta theta_cor = LcwExample1();

        REQUIRE(theta_parsed == theta_cor);
    }

    SECTION("MspCreator", "Test the methods for creating an Msp from a Theta-based structure") {
        Msp msp1 = mspCreator.createWithLcwAlgorithm(LcwExample1());

        Mat<ZZ_p> M_correct;
        M_correct.SetDims(12,7);
        Vec<ZZ_p> row; row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); 
        M_correct[0] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(2)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); 
        M_correct[1] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); 
        M_correct[2] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(2)); row.append(ZZ_p(0)); row.append(ZZ_p(1)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); 
        M_correct[3] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(2)); row.append(ZZ_p(0)); row.append(ZZ_p(2)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); 
        M_correct[4] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(2)); row.append(ZZ_p(0)); row.append(ZZ_p(3)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); 
        M_correct[5] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(1)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); 
        M_correct[6] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(2)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); 
        M_correct[7] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(3)); row.append(ZZ_p(1)); row.append(ZZ_p(1)); 
        M_correct[8] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(3)); row.append(ZZ_p(2)); row.append(ZZ_p(4)); 
        M_correct[9] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(3)); row.append(ZZ_p(3)); row.append(ZZ_p(9)); 
        M_correct[10] = row;
        row.kill();  row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(0)); row.append(ZZ_p(0)); row.append(ZZ_p(3)); row.append(ZZ_p(4)); row.append(ZZ_p(16)); 
        M_correct[11] = row;

        vector<hotstuff::ReplicaID> L_correct{1,2,3,4,5,6,7,8,9,10,11,12};
        vec_ZZ_p e1_correct; 
        e1_correct.append(ZZ_p(1)); e1_correct.append(ZZ_p(0)); e1_correct.append(ZZ_p(0)); e1_correct.append(ZZ_p(0)); e1_correct.append(ZZ_p(0)); e1_correct.append(ZZ_p(0)); e1_correct.append(ZZ_p(0));

        REQUIRE(msp1.M == M_correct);
        REQUIRE(msp1.L == L_correct);
        REQUIRE(msp1.m() == 12);
        REQUIRE(msp1.d() == 7);
        REQUIRE(msp1.e1() == e1_correct);
    }  

    SECTION("isAuthorized", "Test the methods for creating an Msp from a Theta-based structure") {
        Msp msp = mspCreator.createWithLcwAlgorithm(SimpleThreshold1());
        REQUIRE(msp.m() == 4);
        REQUIRE(msp.d() == 2);

        //empty set
        unordered_set<hotstuff::ReplicaID> reps{};
        
        NTL::mat_ZZ_p Ma = msp.getRowsOwnedByGroup(reps);
        REQUIRE(Ma.NumRows() == 0);
        REQUIRE(Ma.NumCols() == 2);
        
        mat_ZZ_p MaT = transpose(Ma);
        REQUIRE(MaT.NumRows() == 2);
        REQUIRE(MaT.NumCols() == 0);

        mat_ZZ_p augm = msp.getAugmentedMatrix(MaT, msp.e1());
        mat_ZZ_p augm_correct;
        augm_correct.SetDims(2, 1);
        Vec<ZZ_p> row; row.append(ZZ_p(1)); augm_correct[0] = row; 
        row.kill(); row.append(ZZ_p(0)); augm_correct[1] = row; 
        REQUIRE(augm == augm_correct);

        int deg_Ma_T = NTL::gauss(MaT);
        int deg_Ma_T_augm = NTL::gauss(augm);
        REQUIRE(deg_Ma_T == 0);
        REQUIRE(deg_Ma_T_augm == 1);

        REQUIRE(msp.isAuthorisedGroup(reps) == false);
        

        //set {1}, unauthorized
        reps = {1};
        Ma = msp.getRowsOwnedByGroup(reps);
        mat_ZZ_p Ma_correct; Ma_correct.SetDims(1,2);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); Ma_correct[0] = row; 
        REQUIRE(Ma == Ma_correct);

        MaT = transpose(Ma);
        mat_ZZ_p MaT_correct;; MaT_correct.SetDims(2, 1);
        row.kill(); row.append(ZZ_p(1)); MaT_correct[0] = row; 
        row.kill(); row.append(ZZ_p(1)); MaT_correct[1] = row; 
        REQUIRE(MaT == MaT_correct);

        augm = msp.getAugmentedMatrix(MaT, msp.e1());
        augm_correct.kill(); augm_correct.SetDims(2, 2);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); augm_correct[0] = row; 
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(0)); augm_correct[1] = row; 
        REQUIRE(augm == augm_correct);

        deg_Ma_T = NTL::gauss(MaT);
        deg_Ma_T_augm = NTL::gauss(augm);
        REQUIRE(deg_Ma_T == 1);
        REQUIRE(deg_Ma_T_augm == 2);

        REQUIRE(msp.isAuthorisedGroup(reps) == false);

        
        //set {1, 3, 4}, authorized
        reps = std::unordered_set<hotstuff::ReplicaID>{1,3,4};
        Ma = msp.getRowsOwnedByGroup(reps);
        Ma_correct.kill(); Ma_correct.SetDims(3,2);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); Ma_correct[0] = row; 
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(3)); Ma_correct[1] = row; 
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(4)); Ma_correct[2] = row; 
        REQUIRE(Ma == Ma_correct);
    
        MaT = transpose(Ma);
        MaT_correct.kill(); MaT_correct.SetDims(2, 3);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(1)); MaT_correct[0] = row; 
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(4)); MaT_correct[1] = row; 
        REQUIRE(MaT == MaT_correct);

        augm = msp.getAugmentedMatrix(MaT, msp.e1());
        augm_correct.kill(); augm_correct.SetDims(2, 4);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(1)); augm_correct[0] = row; 
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(4)); row.append(ZZ_p(0)); augm_correct[1] = row; 
        REQUIRE(augm == augm_correct);

        deg_Ma_T = NTL::gauss(MaT);
        deg_Ma_T_augm = NTL::gauss(augm);
        REQUIRE(deg_Ma_T == 2);
        REQUIRE(deg_Ma_T_augm == 2);

        REQUIRE(msp.isAuthorisedGroup(reps) == true);


        msp = mspCreator.createWithLcwAlgorithm(LcwExample1());   
        REQUIRE(msp.isAuthorisedGroup({}) == false);
        REQUIRE(msp.isAuthorisedGroup({1,2,3}) == false);
        REQUIRE(msp.isAuthorisedGroup({1,2,3,4}) == false);
        REQUIRE(msp.isAuthorisedGroup({1,2,4,5}) == true);
        REQUIRE(msp.isAuthorisedGroup({1,2,4,5,6}) == true);
        REQUIRE(msp.isAuthorisedGroup({1,2,7,8}) == true);
        REQUIRE(msp.isAuthorisedGroup({1,2,7,9}) == false);
        REQUIRE(msp.isAuthorisedGroup({1,2,7,9, 10}) == false);
        REQUIRE(msp.isAuthorisedGroup({4,9,10,11}) == false);
        REQUIRE(msp.isAuthorisedGroup({4,6,9,10,11}) == false);
        REQUIRE(msp.isAuthorisedGroup({4,6,8,9,10,11}) == true);

    }
}


