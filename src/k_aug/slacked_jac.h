//
// Created by dav0 on 8/29/18.
//

#ifndef K_AUG_SLACKED_JAC_H
#define K_AUG_SLACKED_JAC_H

#include "../../thirdparty/asl/solvers/asl.h"
#include "k_aug_data.h"

/* ASL *asl, real *x, fint *Acol, fint *Arow, real *Aij,
                     int nvar, int ncon, fint nzc_, fint *nerror, int **nz_row_a */
void slacked_jac(ASL *asl, nlp_info *nlp_i, real *x, nlp_pd *nlp_pd0);
void count_nz_jac(cgrad **cgrad1, nlp_info *nlp_info1);

typedef struct triplet_grad{
    int r;
    double *a_ptr;
    double a;
}t_g;

void reorder_grad(t_g *grad_s, int nz_g, int **ptr);
int grad_elem_comparision(const void *t1, const void *t2);

void sl_grad_times_y(const nlp_info *nlp_i, int *cptr, int *rw, double *a, double *y, double *dcy);




#endif //K_AUG_SLACKED_GRAD_H
