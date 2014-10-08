/*Copyright (c) 2011, Edgar Solomonik, all rights reserved.*/

#include "../shared/util.h"
#include <limits.h>
#include "sym_seq_sum.h"

namespace CTF_int {
  int sym_seq_sum_cust(char const *         A,
                       semiring             isA,
                       int                  order_A,
                       int const *          edge_len_A,
                       int const *          _lda_A,
                       int const *          sym_A,
                       int const *          idx_map_A,
                       char *               B,
                       semiring             isB,
                       int                  order_B,
                       int const *          edge_len_B,
                       int const *          _lda_B,
                       int const *          sym_B,
                       int const *          idx_map_B,
                       univar_function      func){
    AU_FSTART(sym_seq_sum_cust);
    int idx, i, idx_max, imin, imax, idx_A, idx_B, iA, iB, j, k;
    int off_idx, off_lda, sym_pass;
    int * idx_glb, * rev_idx_map;
    int * dlen_A, * dlen_B;

    inv_idx(order_A,       idx_map_A,
            order_B,       idx_map_B,
            &idx_max,     &rev_idx_map);

    dlen_A = (int*)CTF_alloc(sizeof(int)*order_A);
    dlen_B = (int*)CTF_alloc(sizeof(int)*order_B);
    memcpy(dlen_A, edge_len_A, sizeof(int)*order_A);
    memcpy(dlen_B, edge_len_B, sizeof(int)*order_B);

    idx_glb = (int*)CTF_alloc(sizeof(int)*idx_max);
    memset(idx_glb, 0, sizeof(int)*idx_max);


    /* Scale B immediately. FIXME: wrong for iterators over subset of B */
  /*  if (beta != 1.0) {
      sz = sy_packed_size(order_B, edge_len_B, sym_B, NULL);
      for (i=0; i<sz; i++){
        B[i] = B[i]*beta;
      }
    }*/
    idx_A = 0, idx_B = 0;
    sym_pass = 1;
    for (;;){
      if (sym_pass){
        func.apply_f(A+idx_A*isA.el_size, B+idx_B*isB.el_size);
        CTF_FLOPS_ADD(2);
      }

      for (idx=0; idx<idx_max; idx++){
        imin = 0, imax = INT_MAX;

        GET_MIN_MAX(A,0,2);
        GET_MIN_MAX(B,1,2);

        ASSERT(idx_glb[idx] >= imin && idx_glb[idx] < imax);

        idx_glb[idx]++;

        if (idx_glb[idx] >= imax){
          idx_glb[idx] = imin;
        }
        if (idx_glb[idx] != imin) {
          break;
        }
      }
      if (idx == idx_max) break;

      CHECK_SYM(A);
      if (!sym_pass) continue;
      CHECK_SYM(B);
      if (!sym_pass) continue;
      
      if (order_A > 0)
        RESET_IDX(A);
      if (order_B > 0)
        RESET_IDX(B);
    }
    CTF_free(dlen_A);
    CTF_free(dlen_B);
    CTF_free(idx_glb);
    CTF_free(rev_idx_map);
    TAU_FSTOP(sym_seq_sum_cust);
    return 0;
  }
}
