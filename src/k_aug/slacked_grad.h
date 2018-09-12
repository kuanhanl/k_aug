//
// Created by dav0 on 9/12/18.
//

#ifndef K_AUG_SLACKED_GRAD_H
#define K_AUG_SLACKED_GRAD_H

#include "../../thirdparty/asl/solvers/asl.h"
#include "k_aug_data.h"

void slacked_grad(ASL *asl, nlp_info *nlp_i, nlp_pd *nlp_pd);

#endif //K_AUG_SLACKED_GRAD_H
