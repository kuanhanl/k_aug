//
// Created by dav0 on 9/12/18.
//

#include "get_mult0.h"

void get_mult0(nlp_info *nlp_i, nlp_pd *nlp_pd) {
    int i, j, d_slk = 0, m, n;
    double *yu = NULL, *yl = NULL, *y = NULL;
    n = nlp_i->n_orig + nlp_i->n_slack;
    m = nlp_i->m_orig + nlp_i->m_glu;

    yu = (double *) malloc(m * sizeof(double)); /* leq inequality */
    yl = (double *) malloc(m * sizeof(double)); /* geq inequality */
    y = (double *) malloc(m * sizeof(double)); /* geq inequality */

    memset(yu, 0, m * sizeof(double));
    memset(yl, 0, m * sizeof(double));
    memset(y, 0, m * sizeof(double));

    d_slk = 0; /* double slack */
    for (i = 0; i < nlp_i->m_orig; i++) {
        if (nlp_i->con_flag[i] == 2) {
            yl[i] = nlp_pd->y_orig[i];
        } else if (nlp_i->con_flag[i] == -1) { /* Double inequality */
            j = nlp_i->m_orig + (d_slk++);
            if (nlp_pd->slack_curr[i] > 0.0 && nlp_pd->slack_curr[j] > 0.0) { ;
            } else if (nlp_pd->slack_curr[i] > 0.0) {
                yu[i] = nlp_pd->y_orig[i];
            } else if (nlp_pd->slack_curr[j] > 0.0) {
                yl[i] = nlp_pd->y_orig[i];
                yl[i + nlp_i->m_orig] = nlp_pd->y_orig[i]; /* trailing part of the multipliers */
                y[i] = nlp_pd->y_orig[i];
            } else {
                fprintf(stderr, "The slacks cannot be both 0 at constraint %d\n", i);
                exit(-1);
            }
        } else { /*leq and equality */
            /* very sloppy */
            yl[i] = nlp_pd->y_orig[i];
            /*yu[i] = nlp_pd->y_orig[i];*/
        }
        y[i] = yl[i] - yu[i];
    }

    nlp_pd->y = y;
    nlp_pd->yu = yu;
    nlp_pd->yl = yl;

}
