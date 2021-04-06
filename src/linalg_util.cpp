#include "hotstuff/linalg_util.h"

namespace hotstuff::quorums::linalg{

//In comparison to the original mat_ZZ_p::gauss() in NTL, this function takes advantage of the fact that 
//M_in is almost-upper-triangular (stops if 0 under pivot)
 std::vector<long> gauss(mat_ZZ_p& M_in) {
   ZZ t1, t2;
   ZZ piv;

   long n = M_in.NumRows();
   long m = M_in.NumCols();

   const ZZ& p = ZZ_p::modulus();

   Vec<ZZVec> M;
   sqr(t1, p);
   mul(t1, t1, n);

   M.SetLength(n);
   for (long i = 0; i < n; i++) {
      M[i].SetSize(m, t1.size());
      for (long j = 0; j < m; j++) {
         M[i][j] = rep(M_in[i][j]);
      }
   }

   long l = 0;
   std::vector<long> pivot_cols;
   for (long k = 0; k < m && l < n; k++) {

      long pos = -1;
      for (long i = l; i < n; i++) {
         rem(t1, M[i][k], p);
         M[i][k] = t1;
         if (pos == -1 && !IsZero(t1)) {
            pos = i;
         }
      }

      if (pos != -1) {      
         swap(M[pos], M[l]);

         InvMod(piv, M[l][k], p);
         NegateMod(piv, piv, p);

         for (long j = k+1; j < m; j++) {
            rem(M[l][j], M[l][j], p);
         }

         ZZ t1, t2;

         for (long i = l+1; i<n; i++) {
            if (M[i][k] != 0){
               // M[i] = M[i] + M[l]*M[i,k]*piv
               MulMod(t1, M[i][k], piv, p);
   
               clear(M[i][k]);
   
               ZZ *x = M[i].elts() + (k+1);
               ZZ *y = M[l].elts() + (k+1);
   
               for (long j = k+1; j < m; j++, x++, y++) {
                  // *x = *x + (*y)*t1
                  mul(t2, *y, t1);
                  add(t2, t2, *x);
                  *x = t2;
               }
               
            }
         }

         l++;
         pivot_cols.push_back(k);
      }
   }
   
   for (long i = 0; i < n; i++)
      for (long j = 0; j < m; j++)
         conv(M_in[i][j], M[i][j]);

   return pivot_cols;
}


void PLU(const mat_ZZ_p& M_in, mat_ZZ_p& P, mat_ZZ_p& L, mat_ZZ_p& U) {
// Does not affect M_in. Updates P, L, U so that PM = LU.
   ZZ t1, t2;
   ZZ piv;

   long n = M_in.NumRows();
   long m = M_in.NumCols();

   const ZZ& p = ZZ_p::modulus();

   Vec<ZZVec> M;
   sqr(t1, p);
   mul(t1, t1, n);

   M.SetLength(n);
   for (long i = 0; i < n; i++) {
      M[i].SetSize(m, t1.size());
      for (long j = 0; j < m; j++) {
         M[i][j] = rep(M_in[i][j]);
      }
   }

   P.kill(); ident(P, n);
   L.kill(); ident(L, n);
   U.kill(); U.SetDims(n,m);

   long l = 0;
   for (long k = 0; k < m && l < n; k++) {

      long pos = -1;
      for (long i = l; i < n; i++) {
         rem(t1, M[i][k], p);
         M[i][k] = t1;
         if (pos == -1 && !IsZero(t1)) {
            pos = i;
         }
      }

      if (pos != -1) {      
         if (pos != l){
            swap(M[pos], M[l]);
            swap(P[pos], P[l]);
            swap(L[pos], L[l]);
            for (long ii = 0; ii < n; ii++)
                swap(L[ii][pos], L[ii][l]);
         }

         InvMod(piv, M[l][k], p);
         NegateMod(piv, piv, p);

         for (long j = k+1; j < m; j++) {
            rem(M[l][j], M[l][j], p);
         }

         ZZ t1, t2;

         for (long i = l+1; i<n; i++) {
            // M[i] = M[i] + M[l]*M[i,k]*piv
            MulMod(t1, M[i][k], piv, p);

            clear(M[i][k]);

            ZZ *x = M[i].elts() + (k+1);
            ZZ *y = M[l].elts() + (k+1);

            for (long j = k+1; j < m; j++, x++, y++) {
               // *x = *x + (*y)*t1
               mul(t2, *y, t1);
               add(t2, t2, *x);
               *x = t2;
            }

            NegateMod(t1, t1, p);
            conv(L[i][l], t1);
         }

         l++;
      }
   }
   
   for (long i = 0; i < n; i++)
      for (long j = 0; j < m; j++)
         conv(U[i][j], M[i][j]);
}


void solveByForwardSubstitution(const mat_ZZ_p& L, vec_ZZ_p& x, const vec_ZZ_p& b){
   //Solves Lx=b. L is assumed to be a lower triangular square matrix.
   long n = L.NumCols();
   ZZ_p sum, linv;
   
   x.kill(); x.SetLength(n);
   for (long i = 0; i < n; i++){
      sum = 0;
      for (long j = 0; j < i; j++)
         sum += L[i][j] * x[j];
      inv(linv, L[i][i]);  
      x[i] = (b[i] - sum) * linv;
   }
}

inline bool isPivot(const mat_ZZ_p& A, const long i, const long j){
   if (A[i][j] == 0) return  false;
   for (long jj = j-1; jj >= 0; jj--)
      if (A[i][jj] != 0) return false;
   return true;
}

vec_ZZ_p solveByBackwardSubstitution(const mat_ZZ_p& M, std::vector<long> pivot_cols){
   // M is the augmented A|b in row echelon. Solves Ax=b by backward substitution. A has dimensions (n,m).
   // Parameter rank is the rank of A|b (same as rank of A, because the system is assumed to have solutions). 
   // (rank = #pivots = #non-zero rows).
   // The algorithm assignes 0 to free variables.
   long m = M.NumCols() - 1; // Last column is b
   ZZ_p sum, linv;
   vec_ZZ_p x;
   x.SetLength(m);
   long i = pivot_cols.size() - 1; //only the first (rank) rows are non-zero
   // for (long j = m - 1; j >= 0 ; j--){
      // if (isPivot(M, i, j)){
   for (std::vector<long>::reverse_iterator it = pivot_cols.rbegin(); it != pivot_cols.rend(); it++ ){
      long j = *it;
      sum = 0;
      for (long k = j + 1; k < m; k++)
         sum += M[i][k] * x[k];
      inv(linv, M[i][j]);  
      x[j] = (M[i][m] - sum) * linv; //M[i][m] is b[i]
      i--;
      // } else {
      //    x[j] = 0;
      // }
   }
   return x;
}

}