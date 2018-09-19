//
// Created by dav0 on 8/31/18.
//

#include <assert.h>
#include "slacked_hessian.h"

void slacked_hessian(ASL *asl, nlp_info *nlp_i, nlp_pd *nlp_pd) {
    int i, j, k, d_slk = 0, nz_h, nz_slk_h, md_z, m, n;
    int *hr = NULL, *col_ptr = NULL;
    double *h = NULL;
    int obj_sign = 1;
    double obj_weight = 1.0;
    double *y = NULL;

    if (asl->i.n_obj_ == 0) { exit(-1); }

    nz_h = asl->p.Sphset(asl, &(asl->i.sputinfo_), 0, obj_sign, 1, 1);

    printf("nz_h %d\n", nz_h);

    n = nlp_i->n_orig + nlp_i->n_slack;

    y = nlp_pd->y_asl;

    nz_slk_h = nz_h + nlp_i->n_slack + nlp_i->n_orig;

    hr = (int *) malloc(sizeof(int) * nz_slk_h);
    col_ptr = (int *) malloc(sizeof(int) * (n + 1));
    memset(col_ptr, 0, (n + 1) * sizeof(int));
    h = (double *) malloc(sizeof(double) * nz_slk_h);

    asl->p.Sphes(asl, &(asl->i.sputinfo_), h, 0, &obj_weight, y);
    /* access column by column */
    k = 0;
    for (i = 0; i < nlp_i->n_orig; i++) {
        col_ptr[i] = k;
        for (j = (asl->i.sputinfo_)->hcolstarts[i]; j < (asl->i.sputinfo_)->hcolstarts[i + 1]; j++) {
            hr[k] = (asl->i.sputinfo_)->hrownos[j];
            h[k] = h[j];
            k++;
        }
        /* Check if there is an nz in the main diagonal */
        j = (asl->i.sputinfo_)->hcolstarts[i+1] - 1; /*col end*/
        /* j will not be NULL */
        if(i != (asl->i.sputinfo_)->hrownos[j] ||
        (asl->i.sputinfo_)->hcolstarts[i+1] - (asl->i.sputinfo_)->hcolstarts[i] < 1){
            hr[k] = i;
            h[k] = 0.0;
            k++;
        }
    }
    /* slacks' block */
    for (i = 0; i < nlp_i->n_slack; i++){
        j = i + nlp_i->n_orig;
        col_ptr[j] = k;
        hr[k] = j;
        h[k] = 0.0;
        k++;
    }
    col_ptr[nlp_i->n_slack + nlp_i->n_orig] = k;

    printf("Блин! Allocated:\t%d\tactual nz:\t%d\n", nz_slk_h, k);

    nlp_pd->hess_l = h;
    nlp_pd->hl_r = hr;
    nlp_pd->hlptr = col_ptr;

}

