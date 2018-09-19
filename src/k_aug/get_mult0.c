//
// Created by dav0 on 9/12/18.
//

#include <assert.h>
#include "get_mult0.h"

void get_multy0(nlp_info *nlp_i, nlp_pd *nlp_pd) {
    int i, j, d_slk = 0, m;
    double *yu = NULL, *yl = NULL, *y = NULL;
    m = nlp_i->m_orig + nlp_i->m_glu;

    yu = (double *) malloc(m * sizeof(double)); /* leq inequality */
    yl = (double *) malloc(m * sizeof(double)); /* geq inequality */
    y = (double *) malloc(m * sizeof(double)); /* geq inequality */

    nlp_pd->y_asl = (double *) malloc(nlp_i->m_orig * sizeof(double));

    assert(yu);
    assert(yl);
    assert(y);

    memset(yu, 0, m * sizeof(double));
    memset(yl, 0, m * sizeof(double));
    memset(y, 0, m * sizeof(double));
    memset(nlp_pd->y_asl, 0, nlp_i->m_orig * sizeof(double));

    d_slk = 0; /* double slack */
    for (i = 0; i < nlp_i->m_orig; i++) {
        if (nlp_i->con_flag[i] == 2) {
            yl[i] = nlp_pd->y_orig[i];
            nlp_pd->y_asl[i] = -yl[i];
        } else if (nlp_i->con_flag[i] == -1) { /* Double inequality */
            j = nlp_i->m_orig + (d_slk++);
            if (nlp_pd->sl_curr[i] > 0.0 && nlp_pd->su_curr[j] > 0.0) { /* all zero case */;
            } else if (nlp_pd->sl_curr[i] > 0.0) {
                yu[i] = nlp_pd->y_orig[i]; /* yl = 0 */
                nlp_pd->y_asl[i] = yu[i];

            } else if (nlp_pd->su_curr[j] > 0.0) {
                yl[j] = nlp_pd->y_orig[i]; /* yu = 0 */ /* this goes at the end */
                nlp_pd->y_asl[i] = -yl[i];

            } else {
                fprintf(stderr, "The slacks cannot be both 0 at constraint %d\n", i);
                exit(-1);
            }
            /* we can update z_s here too! */
            continue;
        } else { /*geq and equality */
            yu[i] = nlp_pd->y_orig[i]; /* very sloppy */
            nlp_pd->y_asl[i] = yu[i];
        }
        y[i] = yu[i] - yl[i]; /* the correct multiplier for hessian calculation */
    }
    nlp_pd->y = y;
    nlp_pd->yu = yu;
    nlp_pd->yl = yl;
}

void get_multz0(const nlp_info *nlp_i, nlp_pd *nlp_pd1, double *zl0, double *zu0) {
    /* gets the multipliers of the slacked problem */
    int i = 0, j, n;
    double *zu = NULL, *zl = NULL, *z = NULL;
    FILE *myfile = NULL;
    n = nlp_i->n_orig + nlp_i->n_slack;

    zl = (double *) malloc(sizeof(double) * n);
    zu = (double *) malloc(sizeof(double) * n);
    z = (double *) malloc(sizeof(double) * n);

    memset(zl, 0, sizeof(double) * n);
    memset(zu, 0, sizeof(double) * n);
    memset(z, 0, sizeof(double) * n);

    for (i = 0; i < nlp_i->n_orig; i++) {
        zl[i] = zl0[i];
        zu[i] = zu0[i];
        z[i] = zl[i] + zu[i];
    }
    if (!(nlp_pd1->y) || !(nlp_pd1->yl) || !(nlp_pd1->yu)) {
        fprintf(stderr, "! missing y multipliers");
        nlp_pd1->nlp_pdd(nlp_pd1);
        exit(-1);
    }
    for (i = nlp_i->n_orig; i < n; i++) {
        j = nlp_i->con_slack[i - nlp_i->n_orig];
        printf("j \t %d\n", j);
        zl[i] = nlp_pd1->y[j];
        z[i] = -zl[i];  /* only lower bnds */
    }

    myfile = fopen("z_mult.txt", "w");
    for (i = 0; i < n; i++) {
        fprintf(myfile, "%f\t%f\t%f\n", z[i], zl[i], zu[i]);
    }
    fclose(myfile);

    nlp_pd1->z = z;
    nlp_pd1->zl = zl;
    nlp_pd1->zu = zu;
}

