//
// Created by dav0 on 8/31/18.
//

#include "slacked_hessian.h"

void slacked_hessian(ASL *asl, nlp_info *nlp_i, double *x, double *y) {
    int i, j, k, nz_h;
    int *hr = NULL, *hc = NULL;
    double *h = NULL;
    int obj_sign = 1;
    double obj_weight = 1.0;

    if (asl->i.n_obj_ == 0) {
        exit(-1);
    }
    nz_h = asl->p.Sphset(asl, &(asl->i.sputinfo_), 0, obj_sign, 1, 1);
    printf("nz_h %d\n", nz_h);


    hr = (int *) malloc(sizeof(int) * nz_h);
    hc = (int *) malloc(sizeof(int) * nz_h);
    h = (double *) malloc(sizeof(double) * nz_h);

    asl->p.Sphes(asl, &(asl->i.sputinfo_), h, 0, &obj_weight, y);

    for (i = 0; i < nlp_i->n; i++) {
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
}
