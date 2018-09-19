//
// Created by dav0 on 9/3/18.
//

#ifndef K_AUG_COMPUTE_SLACK_H
#define K_AUG_COMPUTE_SLACK_H

#include "../../thirdparty/asl/solvers/asl.h"
#include "k_aug_data.h"

void compute_slacks(ASL *asl, const nlp_info *nlp_info1, nlp_pd *nlp_pd1);

#endif //K_AUG_COMPUTE_SLACK_H
