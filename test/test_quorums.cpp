#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "hotstuff/quorums.h"
#include "hotstuff/linalg_util.h"
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/matrix.h>

#include <bls/bls384_256.h>
#include <bls/bls.hpp>

using namespace std;
using namespace hotstuff::quorums;
using namespace NTL;

Theta InsertionExample1(){
    Theta theta1 = {2, {1, 2, 3}, {}};
    Theta theta2 = {2, {4, 5, 6}, {}};
    Theta theta3 = {3, {9, 10, 11, 12}, {}};
    Theta theta4 = {2, {7, 8}, {theta3}};
    return {2, {}, {theta1, theta2, theta4}};
}

std::string InsertionExample1StringConf = " { \"select\": 2, "
            " \"out-of\": [ {\"select\": 2, \"out-of\":[1, 2, 3]} , "
            "{\"select\": 2, \"out-of\":[4, 5, 6]} , "
            "{\"select\": 2, \"out-of\":[7, 8, {\"select\": 3, \"out-of\":[9, 10, 11, 12] } ]} "
           "] }";

Theta SimpleThreshold1(){
    return {2, {1, 2, 3, 4}, {}};
}

std::string one_common_k5_StringConf =
       "{\"select\": 4, \"out-of\": [ "
       "     {\"select\": 2, \"out-of\": [0, {\"select\": 2, \"out-of\": [5,6,7,8]} ]}, "
       "     {\"select\": 2, \"out-of\": [1, {\"select\": 2, \"out-of\": [8,9,10,11]} ]}, "
       "     {\"select\": 2, \"out-of\": [2, {\"select\": 2, \"out-of\": [11,12,13,14]} ]}, "
       "     {\"select\": 2, \"out-of\": [3, {\"select\": 2, \"out-of\": [14,15,16,17]} ]}, "
       "     {\"select\": 2, \"out-of\": [4, {\"select\": 2, \"out-of\": [17,18,19,5]} ]} "
       " ]} ";

void test_recomb(hotstuff::quorums::Msp msp, std::unordered_set<hotstuff::ReplicaID> A){
    ZZ_p x; vec_ZZ_p s; std::vector<hotstuff::ReplicaID> L;
    msp.shareRandSecret(x, s, L);
    vec_ZZ_p s_A;
    for (int i = 0; i < s.length(); i++){
        if (A.count(L[i]) > 0){
            s_A.append(s[i]);
        }
    }
    vec_ZZ_p lambda_A = msp.getRecombinationVector(A);
    REQUIRE(lambda_A * s_A == x);
}

TEST_CASE("Linalg util") {
    // NTL::ZZ_p::init(NTL::ZZ(QUORUMS_PRIME_P));
    bls::init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
    std::string FrOrderStr;
    bls::getCurveOrder(FrOrderStr);
    NTL::ZZ_p::init(NTL::ZZ(NTL::INIT_VAL, FrOrderStr.c_str()));

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
    bls::init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
    std::string FrOrderStr;
    bls::getCurveOrder(FrOrderStr);
    NTL::ZZ_p::init(NTL::ZZ(NTL::INIT_VAL, FrOrderStr.c_str()));

    hotstuff::quorums::StringJsonParser jp;
    hotstuff::quorums::InsertionMspCreator mspCreator;
    
     SECTION("JsonParser", "Test the methods for parsing a Json string to a Theta-based structure" ) {    
        Theta theta_parsed = jp.parse(InsertionExample1StringConf);
        Theta theta_cor = InsertionExample1();
        REQUIRE(theta_parsed == theta_cor);
    }

    SECTION("MspCreator", "Test the methods for creating an Msp from a Theta-based structure") {
        Msp msp1 = mspCreator.create(InsertionExample1());

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

        Mat<ZZ_p> V1 = mspCreator.getVandermonde(2,2);
        Mat<ZZ_p> V_correct;
        V_correct.SetDims(2, 2);
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(1)); 
        V_correct[0] = row;
        row.kill(); row.append(ZZ_p(1)); row.append(ZZ_p(2)); 
        V_correct[1] = row;
        REQUIRE(V1 == V_correct);
    }  

    SECTION("isAuthorized", "Test the methods for creating an Msp from a Theta-based structure") {
        Msp msp = mspCreator.create(SimpleThreshold1());
        REQUIRE(msp.m() == 4);
        REQUIRE(msp.d() == 2);
        mat_ZZ_p M_temp;
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

        M_temp = MaT; long deg_Ma_T = NTL::gauss(M_temp);
        M_temp = MaT; long deg_Ma_T_alt = linalg::gauss(M_temp).size();
        REQUIRE(deg_Ma_T == deg_Ma_T_alt);
        M_temp = augm; long deg_Ma_T_augm = NTL::gauss(M_temp);
        M_temp = augm; long deg_Ma_T_augm_alt = linalg::gauss(M_temp).size();
        REQUIRE(deg_Ma_T_augm == deg_Ma_T_augm_alt);

        REQUIRE(deg_Ma_T == 0);
        REQUIRE(deg_Ma_T_augm == 1);

        REQUIRE(msp.isAuthorizedGroupWithoutPLU(reps) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU(reps) == false);
        

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

        M_temp = MaT; deg_Ma_T = NTL::gauss(M_temp);
        M_temp = MaT; deg_Ma_T_alt = linalg::gauss(M_temp).size();
        REQUIRE(deg_Ma_T == deg_Ma_T_alt);
        M_temp = augm; deg_Ma_T_augm = NTL::gauss(M_temp);
        M_temp = augm; deg_Ma_T_augm_alt = linalg::gauss(M_temp).size();
        REQUIRE(deg_Ma_T_augm == deg_Ma_T_augm_alt);

        REQUIRE(deg_Ma_T == 1);
        REQUIRE(deg_Ma_T_augm == 2);

        REQUIRE(msp.isAuthorizedGroupWithoutPLU(reps) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU(reps) == false);

        
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

        M_temp = MaT; deg_Ma_T = NTL::gauss(M_temp);
        M_temp = MaT; deg_Ma_T_alt = linalg::gauss(M_temp).size();
        REQUIRE(deg_Ma_T == deg_Ma_T_alt);
        M_temp = augm; deg_Ma_T_augm = NTL::gauss(M_temp);
        M_temp = augm; deg_Ma_T_augm_alt = linalg::gauss(M_temp).size();
        REQUIRE(deg_Ma_T_augm == deg_Ma_T_augm_alt);

        REQUIRE(deg_Ma_T == 2);
        REQUIRE(deg_Ma_T_augm == 2);

        REQUIRE(msp.isAuthorizedGroupWithoutPLU(reps) == true);
        REQUIRE(msp.isAuthorizedGroupWithPLU(reps) == true);


        msp = mspCreator.create(InsertionExample1());   
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({}) == false);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({1,2,3}) == false);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({1,2,3,4}) == false);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({1,2,4,5}) == true);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({1,2,4,5,6}) == true);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({1,2,7,8}) == true);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({1,2,7,9}) == false);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({1,2,7,9, 10}) == false);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({4,9,10,11}) == false);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({4,6,9,10,11}) == false);
        REQUIRE(msp.isAuthorizedGroupWithoutPLU({4,6,8,9,10,11}) == true);

        REQUIRE(msp.isAuthorizedGroupWithPLU({}) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU({1,2,3}) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU({1,2,3,4}) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU({1,2,4,5}) == true);
        REQUIRE(msp.isAuthorizedGroupWithPLU({1,2,4,5,6}) == true);
        REQUIRE(msp.isAuthorizedGroupWithPLU({1,2,7,8}) == true);
        REQUIRE(msp.isAuthorizedGroupWithPLU({1,2,7,9}) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU({1,2,7,9, 10}) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU({4,9,10,11}) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU({4,6,9,10,11}) == false);
        REQUIRE(msp.isAuthorizedGroupWithPLU({4,6,8,9,10,11}) == true);     

        hotstuff::quorums::StringJsonParser jp;
        msp = mspCreator.create(jp.parse(one_common_k5_StringConf));
       
        REQUIRE(msp.isAuthorizedGroup({0,1,2,3,7,8,9,13,14,15}) == true);
        REQUIRE(msp.isAuthorizedGroup({0,1,2,3,6, 7, 9, 10, 12, 13, 15, 16}) == true);
        REQUIRE(msp.isAuthorizedGroup({0,1,2,3, 5, 8, 11, 14, 17}) == true);

    }

    SECTION("getRecombinationVector") {
        //FORMAT: SECTION("") test_recomb(msp, );

        Msp msp = mspCreator.create(InsertionExample1());
        SECTION("{1,2,4,5}") test_recomb(msp, {1,2,4,5});
        SECTION("{1,2,4,5,6}") test_recomb(msp, {1,2,4,5,6});
        SECTION("{1,2,4,5,6,7}") test_recomb(msp, {1,2,4,5,6,7});
        SECTION("{1,2,4,5,6,7,8}") test_recomb(msp, {1,2,4,5,6,7,8});
        SECTION("{1,2,4,5,6,7,8,9}") test_recomb(msp, {1,2,4,5,6,7,8,9});
        SECTION("{1,2,4,5,6,8,9}") test_recomb(msp, {1,2,4,5,6,8,9});
        SECTION("{4,6,8,9,10,11}") test_recomb(msp, {4,6,8,9,10,11});
        SECTION("{1,2,3,4,6,8,9,10,11}") test_recomb(msp, {1,2,3,4,6,8,9,10,11});

        msp = mspCreator.create(jp.parse(one_common_k5_StringConf));
        SECTION("{0,1,2,3,7,8,9,13,14,15}") test_recomb(msp, {0,1,2,3,7,8,9,13,14,15});
        SECTION("{0,1,2,3,6, 7, 9, 10, 12, 13, 15, 16}") test_recomb(msp, {0,1,2,3,6, 7, 9, 10, 12, 13, 15, 16});
        SECTION("{0,1,2,3, 5, 8, 11, 14, 17}") test_recomb(msp, {0,1,2,3, 5, 8, 11, 14, 17});
        SECTION("{0,1,2,3,4,5, 8,9,11,12,14,16,17}") test_recomb(msp, {0,1,2,3,4,5, 8,9,11,12,14,16,17});

        msp = mspCreator.create(jp.parse(InsertionExample1StringConf));
        SECTION("{1,2,3,4,5,6,7,8,9,10,11}") test_recomb(msp, {1,2,3,4,5,6,7,8,9,10,11});
    }
    
    SECTION("evalFomula", "Evaluate a threshold operator-based formula based on a given (possibly a quorum) set.") {
        //Simple threshold access structure
        hotstuff::quorums::AccessStructure as;
        hotstuff::quorums::Theta formula = SimpleThreshold1();
        //empty set
        unordered_set<hotstuff::ReplicaID> reps{};
        REQUIRE(as.evalFomula(formula, reps) == false);
        //set {1}, unauthorized
        reps = {1};
        REQUIRE(as.evalFomula(formula, reps) == false);
        //set {1, 3, 4}, authorized
        reps = std::unordered_set<hotstuff::ReplicaID>{1,3,4};
        REQUIRE(as.evalFomula(formula, reps) == true);
        //set {1, 2, 3, 4}, authorized
        reps = std::unordered_set<hotstuff::ReplicaID>{1,2,3,4};
        REQUIRE(as.evalFomula(formula, reps) == true);

        //Nested threshold operators
        hotstuff::quorums::JsonParser *jp = new hotstuff::quorums::StringJsonParser();
        as = hotstuff::quorums::AccessStructure(jp);
        as.initialize(InsertionExample1StringConf);
        REQUIRE(as.evalFomula({}) == false);
        REQUIRE(as.evalFomula({1,2,3}) == false);
        REQUIRE(as.evalFomula({1,2,3,4}) == false);
        REQUIRE(as.evalFomula({1,2,4,5}) == true);
        REQUIRE(as.evalFomula({1,2,4,5,6}) == true);
        REQUIRE(as.evalFomula({1,2,7,8}) == true);
        REQUIRE(as.evalFomula({1,2,7,9}) == false);
        REQUIRE(as.evalFomula({1,2,7,9, 10}) == false);
        REQUIRE(as.evalFomula({4,9,10,11}) == false);
        REQUIRE(as.evalFomula({4,6,9,10,11}) == false);
        REQUIRE(as.evalFomula({4,6,8,9,10,11}) == true);

        //1Common example
        as.initialize(one_common_k5_StringConf);
        REQUIRE(as.evalFomula(formula, {0,1,2,3,7,8,9,13,14,15}) == true);
        REQUIRE(as.evalFomula(formula, {0,1,2,3,6, 7, 9, 10, 12, 13, 15, 16}) == true);
        REQUIRE(as.evalFomula(formula, {0,1,2,3, 5, 8, 11, 14, 17}) == true);
        REQUIRE(as.evalFomula(formula, {5, 8, 11, 14, 17}) == false);
    }
    
}

