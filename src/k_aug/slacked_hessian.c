//
// Created by dav0 on 8/31/18.
//

#include <assert.h>
#include "slacked_hessian.h"

void slacked_hessian(ASL *asl, nlp_info *nlp_i, nlp_pd *nlp_pd) {
    int i, j, k, d_slk = 0, nz_h, nz_slk_h, md_z, m, n;
    int *hr = NULL, *hc = NULL;
    double *h = NULL;
    int obj_sign = 1;
    double obj_weight = 1.0;
    double *x = NULL, *yu = NULL, *yl=NULL, *y=NULL;

    if (asl->i.n_obj_ == 0) { exit(-1); }

    nz_h = asl->p.Sphset(asl, &(asl->i.sputinfo_), 0, obj_sign, 1, 1);
    printf("nz_h %d\n", nz_h);

    n = nlp_i->n_orig + nlp_i->n_slack;
    m = nlp_i->m_orig + nlp_i->n_slack;
    x = (double *) malloc(n * sizeof(double));
    yu = (double *) malloc(m * sizeof(double)); /* leq inequality */
    yl = (double *) malloc(m * sizeof(double)); /* geq inequality */
    y = (double *) malloc(m * sizeof(double)); /* geq inequality */

    memset(yu, 0, m * sizeof(double));
    memset(yl, 0, m * sizeof(double));
    memset(y, 0,  m * sizeof(double));

    for (i = 0; i < nlp_i->n_orig; i++) { x[i] = nlp_pd->x_orig[i]; }

    d_slk = 0;
    for (i = 0; i < nlp_i->m_orig; i++) {
        if (nlp_i->con_flag[i] == 2) {
            yl[i] = nlp_pd->y_orig[i];
        } else if (nlp_i->con_flag[i] == -1) { /* Double inequality */
            j = nlp_i->m_orig + (d_slk++);
            if (nlp_pd->slack_curr[i] > 0.0 && nlp_pd->slack_curr[j] > 0.0) {
                ;
            } else if (nlp_pd->slack_curr[i] > 0.0) {
                yu[i] = nlp_pd->y_orig[i];
            } else if (nlp_pd->slack_curr[j] > 0.0) {
                yl[i] = nlp_pd->y_orig[i];
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

    nz_slk_h = nz_h + nlp_i->n_slack + nlp_i->n_orig;

    hr = (int *) malloc(sizeof(int) * nz_slk_h);
    hc = (int *) malloc(sizeof(int) * nz_slk_h);
    h = (double *) malloc(sizeof(double) * nz_slk_h);

    asl->p.Sphes(asl, &(asl->i.sputinfo_), h, 0, &obj_weight, y);
    k = 0;
    for (i = 0; i < nlp_i->n_orig; i++) {
        md_z = 0;
        for (j = (asl->i.sputinfo_)->hcolstarts[i]; j < (asl->i.sputinfo_)->hcolstarts[i + 1]; j++) {
            hr[k] = (asl->i.sputinfo_)->hrownos[j];
            hc[k] = i;
            h[k] = h[j];
            k++;
            md_z++;
        }
        /* Check if there is an nz in the main diagonal */
        j = (asl->i.sputinfo_)->hcolstarts[i+1] - 1; /*col end*/
        /* j will not be NULL */
        printf("i, j %d\t%d\t", i, (asl->i.sputinfo_)->hrownos[j]);
        printf("nz\t%d\n", (asl->i.sputinfo_)->hcolstarts[i+1] - (asl->i.sputinfo_)->hcolstarts[i]);
        /*assert(j);*/
        if(i != (asl->i.sputinfo_)->hrownos[j] ||
        (asl->i.sputinfo_)->hcolstarts[i+1] - (asl->i.sputinfo_)->hcolstarts[i] < 1){
            hr[k] = i;
            hc[k] = i;
            h[k] = 0.0;
            k++;
            printf("appended! %d\t%d\n", i, j);
        }
    }
    for (i = 0; i < nlp_i->n_slack; i++){
        j = i + nlp_i->m_orig;
        hr[k] = j;
        hc[k] = j;
        h[k] = 0.0;
        k++;
    }

    printf("allocated:\t%d\tactual nz:\t%d\n", nz_slk_h, k);

    free(hr);
    free(hc);
    free(h);

    free(x);
    free(y);
    free(yu);
    free(yl);
}
