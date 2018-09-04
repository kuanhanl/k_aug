//
// Created by dav0 on 8/31/18.
//

#include "slacked_hessian.h"

void slacked_hessian(ASL *asl, nlp_info *nlp_i, nlp_pd *nlp_pd) {
    int i, j, k, nz_h;
    int *hr = NULL, *hc = NULL;
    double *h = NULL;
    int obj_sign = 1;
    double obj_weight = 1.0;
    double *x = NULL, *y = NULL;

    if (asl->i.n_obj_ == 0) { exit(-1); }
    nz_h = asl->p.Sphset(asl, &(asl->i.sputinfo_), 0, obj_sign, 1, 1);
    printf("nz_h %d\n", nz_h);

    x = (double *) malloc(nlp_i->n_orig * sizeof(double));
    y = (double *) malloc(nlp_i->m_orig * sizeof(double));

    for (i = 0; i < nlp_i->n_orig; i++) { x[i] = nlp_pd->x_orig[i]; }
    for (i = 0; i < nlp_i->m_orig; i++) {
        if (nlp_i->con_flag[i] == 2) {
            y[i] = -nlp_pd->y_orig[i];
        } else if (nlp_i->con_flag[i] == 3) {
            y[i] = -nlp_pd->y_orig[i];
        }
    }

    hr = (int *) malloc(sizeof(int) * nz_h);
    hc = (int *) malloc(sizeof(int) * nz_h);
    h = (double *) malloc(sizeof(double) * nz_h);

    asl->p.Sphes(asl, &(asl->i.sputinfo_), h, 0, &obj_weight, y);

    for (i = 0; i < nlp_i->n_orig; i++) {
        for (j = (asl->i.sputinfo_)->hcolstarts[i]; j < (asl->i.sputinfo_)->hcolstarts[i + 1]; j++) {
            printf("i %d\tj %d\th %f\n", i, (asl->i.sputinfo_)->hrownos[j], h[j]);
        }
    }

    memset(hr, 0, sizeof(int) * nz_h);
    memset(hc, 0, sizeof(int) * nz_h);
    memset(h, 0, sizeof(double) * nz_h);

    free(hr);
    free(hc);
    free(h);

    free(x);
    free(y);
}
