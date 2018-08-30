//
// Created by dav0 on 8/29/18.
//

#ifndef K_AUG_SLACKED_GRAD_H
#define K_AUG_SLACKED_GRAD_H

#include "../../thirdparty/asl/solvers/asl.h"
#include "k_aug_data.h"

/* ASL *asl, real *x, fint *Acol, fint *Arow, real *Aij,
                     int nvar, int ncon, fint nzc_, fint *nerror, int **nz_row_a */
void slacked_grad(ASL *asl, nlp_info *nlp_i, real *x, int *Acol, int *Arow, double *Aij);


#endif //K_AUG_SLACKED_GRAD_H
