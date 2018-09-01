//
// Created by dav0 on 8/31/18.
//

#ifndef K_AUG_SLACKED_HESSIAN_H
#define K_AUG_SLACKED_HESSIAN_H

#include "../../thirdparty/asl/solvers/asl.h"
#include "k_aug_data.h"

void slacked_hessian(ASL *asl, nlp_info *nlp_i, real *x, real *y);

#endif //K_AUG_SLACKED_HESSIAN_H
