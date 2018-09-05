//
// Created by dav0 on 9/3/18.
//

#include "compute_slacks.h"

void compute_slacks(ASL *asl, nlp_info *nlp_info1, nlp_pd *nlp_pd) {
    int i, d_slk, j = 0;
    double *curr_slacks = NULL;
    asl->p.Xknown(asl, nlp_pd->x_orig, 0);
    nlp_pd->slack_curr = (double *) malloc((nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));
    curr_slacks = nlp_pd->slack_curr;

    memset(curr_slacks, 0, (nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));

    for (i = 0; i < nlp_info1->n_orig; i++) { printf("curr x %f\n", nlp_pd->x_orig[i]); }
    asl->p.Conval(asl, nlp_pd->x_orig, curr_slacks, &i);
    d_slk = 0; /* double slack counter */
    for (i = 0; i < nlp_info1->m_orig; i++) {
        if (nlp_info1->con_flag[i] == 1) {
            curr_slacks[i] = asl->i.LUrhs_[i * 2 + 1] - curr_slacks[i];
        } else if (nlp_info1->con_flag[i] == 2) {
            curr_slacks[i] -= asl->i.LUrhs_[i * 2];
        } else if (nlp_info1->con_flag[i] == -1) {
            /*j = nlp_info1->slack_con[i] + nlp_info1->m_orig;  This points to the last slacks */
            j = nlp_info1->m_orig + (d_slk++);
            curr_slacks[j] = curr_slacks[i];
            curr_slacks[i] = asl->i.LUrhs_[i * 2 + 1] - curr_slacks[i];
            curr_slacks[j] -= asl->i.LUrhs_[i * 2];

        }
    }
    printf("nerror i %d\n", i);
    for (i = 0; i < nlp_info1->m_orig + nlp_info1->m_glu; i++) { printf("slacks %d\t%f\n", i, curr_slacks[i]); }

}
