//
// Created by dav0 on 9/3/18.
//

#include "compute_slacks.h"

void compute_slacks(ASL *asl, nlp_info *nlp_info1, nlp_pd *nlp_pd) {
    int i;
    double *curr_slacks = NULL;
    asl->p.Xknown(asl, nlp_pd->x_orig, 0);
    curr_slacks = (double *) malloc(nlp_info1->m_orig * sizeof(double));
    memset(curr_slacks, 0, nlp_info1->m_orig * sizeof(double));
    for (i = 0; i < nlp_info1->n_orig; i++) { printf("curr x %f\n", nlp_pd->x_orig[i]); }
    asl->p.Conval(asl, nlp_pd->x_orig, curr_slacks, &i);
    for (i = 0; i < nlp_info1->m_orig; i++) {
        if (nlp_info1->con_flag[i] == 1) {
            curr_slacks[i] -= asl->i.LUrhs_[i * 2 + 1];
        } else if (nlp_info1->con_flag[i] == 2) {
            curr_slacks[i] = asl->i.LUrhs_[i * 2] - curr_slacks[i];
        } else if (nlp_info1->con_flag[i] == 3) {
            curr_slacks[i] = asl->i.LUrhs_[i * 2] - curr_slacks[i];
        }

    }
    printf("nerror i %d\n", i);
    for (i = 0; i < nlp_info1->m_orig; i++) { printf("body %f\n", curr_slacks[i]); }
    free(curr_slacks);
}
