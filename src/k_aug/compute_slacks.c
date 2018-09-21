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

    curr_res = (double *) malloc((nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));

    memset(nlp_pd->sl_curr, 0, (nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));
    memset(nlp_pd->su_curr, 0, (nlp_info1->m_orig + nlp_info1->m_glu) * sizeof(double));
    memset(curr_res, 0, nlp_info1->m_orig + nlp_info1->m_glu * sizeof(double));

    asl->p.Conval(asl, nlp_pd->x_orig, curr_res, &i);

    d_slk = 0; /* double slack counter */
    for (i = 0; i < nlp_info1->m_orig; i++) {
        if (nlp_info1->con_flag[i] == 1) {/* upper bound lt */
            nlp_pd->su_curr[i] = asl->i.LUrhs_[i * 2 + 1] - curr_res[i]; /* gU - g(w) */
        } else if (nlp_info1->con_flag[i] == 2) { /* lower bound gt */
            nlp_pd->sl_curr[i] = curr_res[i] - asl->i.LUrhs_[i * 2]; /* g(w) - gL */
        } else if (nlp_info1->con_flag[i] == -1) {
            j = nlp_info1->m_orig + d_slk++;
            nlp_pd->su_curr[i] = asl->i.LUrhs_[i * 2 + 1] - curr_res[i]; /* gU - g(w) */
            nlp_pd->sl_curr[j] = curr_res[i] - asl->i.LUrhs_[i * 2]; /* g(w) - gL */
            curr_res[j] = curr_res[i]; /* initialize this guy  */
        }
    }

    d_slk = 0;
    for(i=0; i < nlp_info1->m_orig; i++){
        if (nlp_info1->con_flag[i] ==  1){
            if (nlp_pd->su_curr[i] == 0.0 && nlp_pd->sl_curr[nlp_info1->m_orig + d_slk++] == 0.0){
                fprintf(stderr, "Both slacks are zero");
                exit(EXIT_FAILURE);}
        }
    }

    myf = fopen("slacks0.txt", "w");
    for (i = 0; i < (nlp_info1->m_orig + nlp_info1->m_glu); i++) {
        fprintf(myf, "%f\t%f\n", nlp_pd->sl_curr[i], nlp_pd->su_curr[i]);
    }
    fclose(myf);


    for (i=0; i < nlp_info1->m_orig; i++){
        /* compute proper residuals */
        if (nlp_info1->con_flag[i] == 1) {/* upper bound lt */
            curr_res[i] = curr_res[i] - asl->i.LUrhs_[i * 2 + 1] + nlp_pd->su_curr[i];
        } else if (nlp_info1->con_flag[i] == 2) { /* lower bound gt */
            curr_res[i] = asl->i.LUrhs_[i * 2] - curr_res[i] + nlp_pd->sl_curr[i];
        } else if (nlp_info1->con_flag[i] == 3) { /* eq */
            curr_res[i] = asl->i.LUrhs_[i * 2] - curr_res[i];
        } else if (nlp_info1->con_flag[i] == 2) {
            curr_res[j] = curr_res[i] - asl->i.LUrhs_[i * 2 + 1] + nlp_pd->su_curr[j]; /* this needs to be first */
            curr_res[i] = asl->i.LUrhs_[i * 2] - curr_res[i] + nlp_pd->sl_curr[i];}
    }
    myf = fopen("res0_.txt", "w");
    for (i = 0; i < (nlp_info1->m_orig + nlp_info1->m_glu); i++) {
        fprintf(myf, "%f\n", curr_res[i]);
    }
    fclose(myf);
    free(curr_res);


}
