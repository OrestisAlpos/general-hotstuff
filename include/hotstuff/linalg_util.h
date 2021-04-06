#ifndef _LINALG_H
#define _LINALG_H

#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/matrix.h>

#include <bls/bls384_256.h>
#include <bls/bls.hpp>
#include <sstream>

using namespace NTL;

namespace hotstuff::quorums::linalg{
    
void PLU(const mat_ZZ_p& M, mat_ZZ_p& P, mat_ZZ_p& L, mat_ZZ_p& U);
void solveByForwardSubstitution(const mat_ZZ_p& L, vec_ZZ_p& x, const vec_ZZ_p& b);
std::vector<long> gauss(mat_ZZ_p& M_in); //returns a vector with the pivot columns
vec_ZZ_p solveByBackwardSubstitution(const mat_ZZ_p& M, std::vector<long> pivot_cols);

}
#endif