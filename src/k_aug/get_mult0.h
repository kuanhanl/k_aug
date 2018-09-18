//
// Created by dav0 on 9/12/18.
//

#ifndef K_AUG_GET_MULT0_H
#define K_AUG_GET_MULT0_H

#include "../../thirdparty/asl/solvers/asl.h"
#include "k_aug_data.h"

void get_multy0(nlp_info *nlp_i, nlp_pd *nlp_pd);

void get_multz0(const nlp_info *nlp_i, nlp_pd *nlp_pd1, double *zl0, double *zu0);

#endif //K_AUG_GET_MULT0_H
