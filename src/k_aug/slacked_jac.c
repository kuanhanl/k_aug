//
// Created by dav0 on 8/29/18.
//

#include "slacked_jac.h"

void slacked_jac(ASL *asl, nlp_info *nlp_i, real *x, int *Acol, int *Arow, double *Aij) {
    int i, j, k, glu_nz, nz_newjac, si = 0;
    int new_m;
    int *nz_c;
    int error = 0;
    double *J = NULL;
    cgrad *cg = NULL;
    int *ac = NULL, *ar = NULL;
    double *a = NULL;
    FILE *my_file;
    glu_nz = 0;

    J = (double *) malloc(sizeof(double) * asl->i.nzc_);
    memset(J, 0, sizeof(double) * asl->i.nzc_);
    jacval(x, J, &error);

    nz_c = (int *) malloc(sizeof(int) * nlp_i->m_orig);

    for (j = 0; j < nlp_i->m_orig; j++) {
        nz_c[j] = 0;
        for (cg = asl->i.Cgrad_[j]; cg; cg = cg->next) {
            nz_c[j]++;
        }
    }

    /* Count the nz necessary for the double ineq constr */
    for (j = 0; j < nlp_i->m_glu; j++) {
        i = nlp_i->glu_c[j];
        glu_nz += nz_c[i];
    }
    nlp_i->n_slack = nlp_i->m_gl + nlp_i->m_gu + nlp_i->m_glu;
    printf("n_slacks %d\n", nlp_i->n_slack);
    printf("Overall missing nz %d\n", glu_nz);
    nz_newjac = asl->i.nzc_ + glu_nz + nlp_i->n_slack + nlp_i->m_glu;
    new_m = nlp_i->m_eq + nlp_i->m_gl + nlp_i->m_gu + 2 * nlp_i->m_glu;
    printf("new nz %d\n", nz_newjac);

    ac = (int *) malloc(sizeof(int) * nz_newjac);
    ar = (int *) malloc(sizeof(int) * nz_newjac);
    a = (double *) malloc(sizeof(double) * nz_newjac);
    memset(a, 0, sizeof(double) * nz_newjac);
    memset(ac, 0, sizeof(int) * nz_newjac);
    memset(ar, 0, sizeof(int) * nz_newjac);

    i = 0;
    si = 0;
    for (j = 0; j < nlp_i->m_orig; j++) {
        for (cg = asl->i.Cgrad_[j]; cg; cg = cg->next) {
            ac[i] = j;
            ar[i] = cg->varno;
            a[i] = (nlp_i->con_flag[j] == 2) ? -J[cg->goff] : J[cg->goff];
            i++;
        }
        if (nlp_i->con_flag[j] != 3) { /* Slacks are required */
            printf("nlp_i->con_flag[%d] = %d\n", j, nlp_i->con_flag[j]);
            ac[i] = j;
            ar[i] = nlp_i->n_orig + si;
            a[i] = 1;
            si++;
            i++;
        }
    }

    /* Append the gl part of this */
    for (j = 0; j < nlp_i->m_glu; j++) {
        k = nlp_i->glu_c[j];
        for (cg = asl->i.Cgrad_[k]; cg; cg = cg->next) {
            ac[i] = nlp_i->m_orig + j;
            ar[i] = cg->varno;
            a[i] = -J[cg->goff];
            i++;
        }
        ac[i] = k;
        ar[i] = nlp_i->n_orig + si;
        a[i] = 1;
        si++;
        i++;
    }

    printf("i number %d\n", i);
    my_file = fopen("local_a.txt", "w");

    for (j = 0; j < nz_newjac; j++) {
        fprintf(my_file, "%d\t%d\t%f\n", ar[j], ac[j], a[j]);
    }
    fclose(my_file);
    my_file = fopen("original_a.txt", "w");
    for (j = 0; j < nlp_i->m_orig; j++) {
        for (cg = asl->i.Cgrad_[j]; cg; cg = cg->next) {
            fprintf(my_file, "%d\t%d\t%f\n", cg->varno, j, J[cg->goff]);
        }
    }
    fclose(my_file);

    free(J);
    free(ac);
    free(ar);
    free(a);
    free(nz_c);

    printf("давай!\n");
}
