#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "hotstuff/quorums.h"
#include "hotstuff/linalg_util.h"
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


TEST_CASE("Linalg util") {
    NTL::ZZ_p::init(NTL::ZZ(QUORUMS_PRIME_P));

    mat_ZZ_p M, P, L, U, PM, LU;
    Vec<ZZ_p> row, x, b, x_correct;
    
    SECTION("PLU"){
        M.SetDims(3,3);
        M[0][0] = 0; M[0][1] = 1; M[0][2] = -2; 
        M[1][0] = 3; M[1][1] = 0; M[1][2] = 1; 
        M[2][0] = 1; M[2][1] = 2; M[2][2] = 2; 

        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        

        M.kill(); M.SetDims(3,3);
        M[0][0] = 1; M[0][1] = 2; M[0][2] = 1; 
        M[1][0] = 1; M[1][1] = 2; M[1][2] = 2; 
        M[2][0] = 2; M[2][1] = 1; M[2][2] = 2; 
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);
        

        M.kill(); M.SetDims(3,5);
        M[0][0] = 1; M[0][1] = 2; M[0][2] = 1;  M[0][3] = 2;  M[0][4] = 1;
        M[1][0] = 2; M[1][1] = 4; M[1][2] = 2;  M[1][3] = 4;  M[1][4] = 1;
        M[2][0] = 1; M[2][1] = 2; M[2][2] = 1;  M[2][3] = 3;  M[2][4] = 2;
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);


        M.kill(); M.SetDims(4,3);
        M[0][0] = 1; M[0][1] = 2; M[0][2] = 1; 
        M[1][0] = 1; M[1][1] = 2; M[1][2] = 2; 
        M[2][0] = 2; M[2][1] = 4; M[2][2] = 1; 
        M[3][0] = 3; M[3][1] = 2; M[3][2] = 1; 
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);


        M.kill(); M.SetDims(3,3);
        M[0][0] = 5;  M[0][1] = 10; M[0][2] = 1; 
        M[1][0] = 1;  M[1][1] = 2;  M[1][2] = 2; 
        M[2][0] = -3; M[2][1] = 4;  M[2][2] = 7; 
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);


        M.kill(); M.SetDims(4,5);
        M[0][0] = 0; M[0][1] = 1; M[0][2] = 0;  M[0][3] = 3;  M[0][4] = -1;
        M[1][0] = 0; M[1][1] = 0; M[1][2] = 2;  M[1][3] = 1;  M[1][4] = 2;
        M[2][0] = 1; M[2][1] = 0; M[2][2] = 1;  M[2][3] = 1;  M[2][4] = 4;
        M[3][0] = 2; M[3][1] = 1; M[3][2] = 0;  M[3][3] = 3;  M[3][4] = 0;
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);


        M.kill(); M.SetDims(1,5);
        M[0][0] = 0; M[0][1] = 1; M[0][2] = 0;  M[0][3] = 3;  M[0][4] = -1;
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);


        M.kill(); M.SetDims(4,1);
        M[0][0] = 1;
        M[1][0] = 1;
        M[2][0] = 2;
        M[3][0] = 3;
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);


        M.kill(); M.SetDims(4,1);
        M[0][0] = 0;
        M[1][0] = 0;
        M[2][0] = 0;
        M[3][0] = 0;
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);

        
        M.kill(); M.SetDims(1,1);
        M[0][0] = 1;
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);


        M.kill(); M.SetDims(1,1);
        M[0][0] = 0;
        
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);


        M.kill(); M.SetDims(0,0);
        hotstuff::quorums::linalg::PLU(M, P, L, U);
        PM = P * M;
        LU = L * U;
        REQUIRE(PM == LU);
        REQUIRE(M == transpose(P) * LU);
    }

    SECTION("solveByForwardSubstitution"){
        L.kill(); L.SetDims(1,1);
        L[0][0] = 1;
        
        b.kill();
        b.append(ZZ_p(2));

        x_correct.kill();
        x_correct.append(ZZ_p(2));

        hotstuff::quorums::linalg::solveByForwardSubstitution(L, x, b);
        REQUIRE(x == x_correct);


        L.kill(); L.SetDims(2,2);
        L[0][0] = 2;
        L[1][0] = 3; L[1][1] = 3;
        
        b.kill();
        b.append(ZZ_p(2)); b.append(ZZ_p(0));

        x_correct.kill();
        x_correct.append(ZZ_p(1)); x_correct.append(ZZ_p(-1));

        hotstuff::quorums::linalg::solveByForwardSubstitution(L, x, b);
        REQUIRE(x == x_correct);
        

        L.kill(); L.SetDims(3,3);
        L[0][0] = 1; 
        L[1][0] = 0;  L[1][1] = 3;
        L[2][0] = -1; L[2][1] = 3; L[2][2] = -3; 
        
        b.kill();
        b.append(ZZ_p(4)); b.append(ZZ_p(-3)); b.append(ZZ_p(-1));

        x_correct.kill(); 
        x_correct.append(ZZ_p(4)); x_correct.append(ZZ_p(-1)); x_correct.append(ZZ_p(-2));

        hotstuff::quorums::linalg::solveByForwardSubstitution(L, x, b);
        REQUIRE(x == x_correct);


        L.kill(); L.SetDims(4,4);
        L[0][0] = 2; 
        L[1][0] = 3;  L[1][1] = -1; 
        L[2][0] = 2;  L[2][1] = 1;  L[2][2] = 1;  
        L[3][0] = -1; L[3][1] = -2; L[3][2] = -2; L[3][3] = 3;
        
        b.kill();
        b.append(ZZ_p(2)); b.append(ZZ_p(1)); b.append(ZZ_p(3)); b.append(ZZ_p(3));

        x_correct.kill();
        x_correct.append(ZZ_p(1)); x_correct.append(ZZ_p(2)); x_correct.append(ZZ_p(-1)); x_correct.append(ZZ_p(2));

        hotstuff::quorums::linalg::solveByForwardSubstitution(L, x, b);
        REQUIRE(x == x_correct);

        
        L.kill(); L.SetDims(0, 0);
        b.kill(); b.SetLength(0);
        x_correct.kill(); x_correct.SetLength(0);
        hotstuff::quorums::linalg::solveByForwardSubstitution(L, x, b);
        REQUIRE(x == x_correct);


        L.kill(); L.SetDims(20,20);
        for (int i = 0; i < 20; i++)
            for (int j = 0; j <= i; j++)
                L[i][j] = random_ZZ_p();

        b.kill(); b.SetLength(20);
        for (int i = 0; i < 20; i++)
            b[i] = random_ZZ_p();
        
        ZZ_p d;
        solve(d, L, x_correct, b);
        if (d != 0){
            hotstuff::quorums::linalg::solveByForwardSubstitution(L, x, b);
            REQUIRE(x == x_correct);
        }

    }
}


TEST_CASE("Quorums") {
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
        Msp msp1 = mspCreator.create(LcwExample1());

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
        Msp msp = mspCreator.create(SimpleThreshold1());
        REQUIRE(msp.m() == 4);
        REQUIRE(msp.d() == 2);

        //empty set
        unordered_set<hotstuff::ReplicaID> reps{};
        
        NTL::mat_ZZ_p Ma = msp.getRowsOfMOwnedByReps(reps);
        REQUIRE(Ma.NumRows() == 0);
        REQUIRE(Ma.NumCols() == 2);
        
        mat_ZZ_p MaT = transpose(Ma);
        REQUIRE(MaT.NumRows() == 2);
        REQUIRE(MaT.NumCols() == 0);

        mat_ZZ_p augm;
        msp.getAugmentedMatrix(augm, MaT, msp.e1());
        mat_ZZ_p augm_correct;
        augm_correct.SetDims(2, 1);
        Vec<ZZ_p> row; row.append(ZZ_p(1)); augm_correct[0] = row; 
        row.kill(); row.append(ZZ_p(0)); augm_correct[1] = row; 
        REQUIRE(augm == augm_correct);

        long deg_Ma_T = NTL::gauss(MaT);
        long deg_Ma_T_alt = linalg::gauss(MaT);
        REQUIRE(deg_Ma_T == deg_Ma_T_alt);
        long deg_Ma_T_augm = NTL::gauss(augm);
        long deg_Ma_T_augm_alt = linalg::gauss(augm);
        REQUIRE(deg_Ma_T_augm == deg_Ma_T_augm_alt);

        REQUIRE(deg_Ma_T == 0);
        REQUIRE(deg_Ma_T_augm == 1);

        REQUIRE(msp.isAuthorisedGroupWithoutPLU(reps) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU(reps) == false);
        

        //set {1}, unauthorized
        reps = {1};
        Ma = msp.getRowsOfMOwnedByReps(reps);
        mat_ZZ_p Ma_correct; Ma_correct.SetDims(1,2);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); Ma_correct[0] = row; 
        REQUIRE(Ma == Ma_correct);

        MaT = transpose(Ma);
        mat_ZZ_p MaT_correct;; MaT_correct.SetDims(2, 1);
        row.kill(); row.append(ZZ_p(1)); MaT_correct[0] = row; 
        row.kill(); row.append(ZZ_p(1)); MaT_correct[1] = row; 
        REQUIRE(MaT == MaT_correct);

        msp.getAugmentedMatrix(augm, MaT, msp.e1());
        augm_correct.kill(); augm_correct.SetDims(2, 2);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); augm_correct[0] = row; 
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(0)); augm_correct[1] = row; 
        REQUIRE(augm == augm_correct);

        deg_Ma_T = NTL::gauss(MaT);
        deg_Ma_T_alt = linalg::gauss(MaT);
        REQUIRE(deg_Ma_T == deg_Ma_T_alt);
        deg_Ma_T_augm = NTL::gauss(augm);
        deg_Ma_T_augm_alt = linalg::gauss(augm);
        REQUIRE(deg_Ma_T_augm == deg_Ma_T_augm_alt);

        REQUIRE(deg_Ma_T == 1);
        REQUIRE(deg_Ma_T_augm == 2);

        REQUIRE(msp.isAuthorisedGroupWithoutPLU(reps) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU(reps) == false);

        
        //set {1, 3, 4}, authorized
        reps = std::unordered_set<hotstuff::ReplicaID>{1,3,4};
        Ma = msp.getRowsOfMOwnedByReps(reps);
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

        msp.getAugmentedMatrix(augm, MaT, msp.e1());
        augm_correct.kill(); augm_correct.SetDims(2, 4);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(1)); row.append(ZZ_p(1)); augm_correct[0] = row; 
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(3)); row.append(ZZ_p(4)); row.append(ZZ_p(0)); augm_correct[1] = row; 
        REQUIRE(augm == augm_correct);

        deg_Ma_T = NTL::gauss(MaT);
        deg_Ma_T_alt = linalg::gauss(MaT);
        REQUIRE(deg_Ma_T == deg_Ma_T_alt);
        deg_Ma_T_augm = NTL::gauss(augm);
        deg_Ma_T_augm_alt = linalg::gauss(augm);
        REQUIRE(deg_Ma_T_augm == deg_Ma_T_augm_alt);

        REQUIRE(deg_Ma_T == 2);
        REQUIRE(deg_Ma_T_augm == 2);

        REQUIRE(msp.isAuthorisedGroupWithoutPLU(reps) == true);
        REQUIRE(msp.isAuthorisedGroupWithPLU(reps) == true);


        msp = mspCreator.create(LcwExample1());   
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({}) == false);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({1,2,3}) == false);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({1,2,3,4}) == false);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({1,2,4,5}) == true);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({1,2,4,5,6}) == true);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({1,2,7,8}) == true);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({1,2,7,9}) == false);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({1,2,7,9, 10}) == false);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({4,9,10,11}) == false);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({4,6,9,10,11}) == false);
        REQUIRE(msp.isAuthorisedGroupWithoutPLU({4,6,8,9,10,11}) == true);

        REQUIRE(msp.isAuthorisedGroupWithPLU({}) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU({1,2,3}) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU({1,2,3,4}) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU({1,2,4,5}) == true);
        REQUIRE(msp.isAuthorisedGroupWithPLU({1,2,4,5,6}) == true);
        REQUIRE(msp.isAuthorisedGroupWithPLU({1,2,7,8}) == true);
        REQUIRE(msp.isAuthorisedGroupWithPLU({1,2,7,9}) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU({1,2,7,9, 10}) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU({4,9,10,11}) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU({4,6,9,10,11}) == false);
        REQUIRE(msp.isAuthorisedGroupWithPLU({4,6,8,9,10,11}) == true);

    }
}

