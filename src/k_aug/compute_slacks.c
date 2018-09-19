//
// Created by dav0 on 9/3/18.
//

#include "compute_slacks.h"

void compute_slacks(ASL *asl, const nlp_info *nlp_info1, nlp_pd *nlp_pd) {
    int i = 0, d_slk, j = 0;
    double *curr_res = NULL;
    FILE *myf = NULL;
    asl->p.Xknown(asl, nlp_pd->x_orig, 0);
    /*nlp_pd->slack_curr = (double *) malloc((nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));*/

    nlp_pd->sl_curr = (double *) malloc((nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));
    nlp_pd->su_curr = (double *) malloc((nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));

    curr_res = (double *) malloc(nlp_info1->m_orig * sizeof(double));

    memset(nlp_pd->sl_curr, 0, (nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));
    memset(nlp_pd->su_curr, 0, (nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));
    memset(curr_res, 0, nlp_info1->m_orig * sizeof(double));

    asl->p.Conval(asl, nlp_pd->x_orig, curr_res, &i);

    d_slk = 0; /* double slack counter */
    for (i = 0; i < nlp_info1->m_orig; i++) {
        if (nlp_info1->con_flag[i] == 1) {/* upper bound */
            nlp_pd->su_curr[i] = asl->i.LUrhs_[i * 2 + 1] - curr_res[i];
        } else if (nlp_info1->con_flag[i] == 2) { /* lower bound */
            nlp_pd->sl_curr[i] = asl->i.LUrhs_[i * 2] - curr_res[i];
        } else if (nlp_info1->con_flag[i] == -1) {
            /*j = nlp_info1->slack_con[i] + nlp_info1->m_orig;  This points to the last slacks */
            j = nlp_info1->m_orig + d_slk++;
            nlp_pd->su_curr[i] = asl->i.LUrhs_[i * 2 + 1] - curr_res[i];
            nlp_pd->sl_curr[j] = asl->i.LUrhs_[i * 2] - curr_res[i];
        }
    }

    myf = fopen("slacks0.txt", "w");
    for (i = 0; i < (nlp_info1->m_orig + nlp_info1->m_glu); i++) {
        fprintf(myf, "%f\t%f\n", nlp_pd->sl_curr[i], nlp_pd->sl_curr[i]);
    }
    fclose(myf);
    free(curr_res);


}
