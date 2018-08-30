//
// Created by dav0 on 8/29/18.
//

#include "slacked_grad.h"

void slacked_grad(ASL *asl, nlp_info *nlp_i, real *x, int *Acol, int *Arow, double *Aij) {
    int i, j, glu_nz, nz_newjac;
    int new_m;
    int *nz_c;
    int error = 0;
    double *J = NULL;
    cgrad *cg = NULL;

    glu_nz = 0;


    J = (double *) malloc(sizeof(double) * asl->i.nzc_);
    memset(J, 0, sizeof(double) * asl->i.nzc_);
    jacval(x, J, &error);

    Acol = (int *) malloc(sizeof(int) * asl->i.nzc_);
    Arow = (int *) malloc(sizeof(int) * asl->i.nzc_);
    Aij = (double *) malloc(sizeof(double) * asl->i.nzc_);
    nz_c = (int *) malloc(sizeof(int) * nlp_i->m);

    for (j = 0; j < nlp_i->m; j++) {
        nz_c[j] = 0;
        for (cg = asl->i.Cgrad_[j]; cg; cg = cg->next) {
            Acol[nz_c[j]] = j;
            Arow[nz_c[j]] = cg->varno;
            Aij[nz_c[j]] = J[cg->goff];
            nz_c[j]++;
        }
    }

    /* Count the nz necesary for the double ineq constr */
    for (j = 0; j < nlp_i->m_glu; j++) {
        i = nlp_i->glu_c[j];
        for (cg = asl->i.Cgrad_[i]; cg; cg = cg->next) {
            glu_nz++;
        }
    }

    printf("missing nz %d\n", glu_nz);

    nz_newjac = asl->i.nzc_ + glu_nz;
    new_m = nlp_i->m_eq + nlp_i->m_gl + nlp_i->m_gu + nlp_i->m_glu;

    free(J);
    free(Acol);
    free(Arow);
    free(Aij);
    free(nz_c);

    printf("давай!\n");
}
