//
// Created by dav0 on 8/29/18.
//

#include <assert.h>
#include "slacked_jac.h"

void slacked_jac(ASL *asl, nlp_info *nlp_i, real *x, nlp_pd *nlp_pd0) {
    int i, j, k, l, m, glu_nz, nz_newjac, si = 0;
    int new_m;
    int *nz_c, *col_ptr = NULL;
    int error = 0;
    double *J = NULL;
    cgrad *cg = NULL;
    int *ar = NULL;
    double *a = NULL, **ap = NULL;
    t_g *grad_s = NULL;
    FILE *my_file;
    glu_nz = 0;

    J = (double *) malloc(sizeof(double) * asl->i.nzc_);
    memset(J, 0, sizeof(double) * asl->i.nzc_);
    grad_s = (t_g *)malloc(sizeof(t_g) * nlp_i->n_orig);

    asl->p.Jacval(asl, nlp_pd0->x_0, J, &error);

    nlp_i->nz_J = (int *)malloc(sizeof(int) * nlp_i->m_orig);
    count_nz_jac(asl->i.Cgrad_, nlp_i);

    /* Count the nz necessary for the double ineq constr */
    for (j = 0; j < nlp_i->m_glu; j++) {
        i = nlp_i->glu_c[j];
        glu_nz += nlp_i->nz_J[i];
    }

    nlp_i->n_slack = nlp_i->m_gl + nlp_i->m_gu + nlp_i->m_glu;
    printf("n_slacks %d\n", nlp_i->n_slack);
    printf("Overall missing nz %d\n", glu_nz);
    nz_newjac = asl->i.nzc_ + glu_nz + nlp_i->n_slack + nlp_i->m_glu;
    new_m = nlp_i->m_eq + nlp_i->m_gl + nlp_i->m_gu + 2 * nlp_i->m_glu;
    printf("new nz %d\n", nz_newjac);

    ar = (int *) malloc(sizeof(int) * nz_newjac);
    a = (double *) malloc(sizeof(double) * nz_newjac);
    col_ptr = (int *) malloc(sizeof(int) * (new_m + 1));
    ap = (double **) malloc(sizeof(double *) * nz_newjac);

    memset(a, 0, sizeof(double) * nz_newjac);
    memset(ar, 0, sizeof(int) * nz_newjac);
    memset(col_ptr, 0, sizeof(int) * (new_m + 1));

    i = 0;
    si = 0;
    m = 0;
    for (j = 0; j < nlp_i->m_orig; j++) {
        k = 0;
        col_ptr[j] = i;
        /* reorder */
        for (cg = asl->i.Cgrad_[j]; cg; cg = cg->next) {
            grad_s[k].a = (nlp_i->con_flag[j] == 2) ? -J[cg->goff] : J[cg->goff];;
            grad_s[k].r = cg->varno;
            grad_s[k].a_ptr = J + cg->goff;
            k++;
        }
        reorder_grad(grad_s, k, NULL);
        for (l = 0; l < k; l++) {
            ar[i] = grad_s[l].r;
            a[i] = grad_s[l].a;
            ap[m++] = a + i;
            i++;
        }
        if (nlp_i->con_flag[j] != 3) { /* Slacks are required */
            ar[i] = nlp_i->n_orig + si;
            a[i] = 1.0;
            si++;
            i++;
        }
    }

    /* Append the gl part of this */
    for (j = 0; j < nlp_i->m_glu; j++) {
        col_ptr[j + nlp_i->m_orig] = i;
        for (l = col_ptr[j]; l < col_ptr[j + 1]; l++) {
            ar[i] = ar[l];
            a[i] = -a[l];
            i++;
        }
        ar[i] = nlp_i->n_orig + si;
        a[i] = 1.0;
        si++;
        i++;
    }

    col_ptr[nlp_i->m_glu + nlp_i->m_orig] = i;

    printf("i number %d\n", i);

    my_file = fopen("original_a.txt", "w");
    for (j = 0; j < nlp_i->m_orig; j++) {
        for (cg = asl->i.Cgrad_[j]; cg; cg = cg->next) {
            fprintf(my_file, "%d\t%d\t%f\n", cg->varno, j, J[cg->goff]);
        }
    }
    fclose(my_file);

    my_file = fopen("jacobian_addresses", "w");
    for (j = 0; j < m; j++) { fprintf(my_file, "%p\t%f\n", ap[j], *(ap[j])); }
    fclose(my_file);

    my_file = fopen("original_b.txt", "w");
    m = 0;
    for (j = 0; j < nlp_i->m_orig; j++) {
        for (cg = asl->i.Cgrad_[j]; cg; cg = cg->next) {
            *(ap[m++]) = J[cg->goff];
        }
    }
    fclose(my_file);

    my_file = fopen("jacobian_addresses_mod", "w");
    for (j = 0; j < m; j++) { fprintf(my_file, "%p\t%f\n", ap[j], *(ap[j])); }
    fclose(my_file);

    nlp_pd0->jac_c = a;
    nlp_pd0->jc_r = ar;
    nlp_pd0->jcptr = col_ptr;

    free(J);

    /*
    free(ar);
    free(a);
    free(col_ptr);
     */

    free(ap);
    free(grad_s);

    printf("давай!\n");
}


void count_nz_jac(cgrad **cgrad1, nlp_info *nlp_info1){
    int j;
    cgrad *g;
    assert(nlp_info1->nz_J);
    memset(nlp_info1->nz_J, 0, sizeof(int) * nlp_info1->m_orig);
    for(j=0; j<nlp_info1->m_orig; j++){
        for(g=cgrad1[j]; g; g = g->next){
            nlp_info1->nz_J[j]++;
        }
    }

}

void reorder_grad(t_g *grad_s, int nz_g, int **ptr) {
    assert(grad_s);
    qsort(grad_s, (size_t) ((unsigned) nz_g), sizeof(t_g), grad_elem_comparision);
}


int grad_elem_comparision(const void *t1, const void *t2) {
    t_g f_t  = *(const t_g *)t1;
    t_g s_t = *(const t_g *)t2;
    int fst = f_t.r;
    int scd = s_t.r;
    return (fst - scd);
}


void sl_grad_times_y(const nlp_info *nlp_i, int *cptr, int *rw, double *a, double *y, double *dcy) {
    int i, j, m;
    m = nlp_i->m_orig + nlp_i->m_glu;
    memset(dcy, 0, sizeof(double) * m);
    for (j = 0; j < m; j++) { /* access by column */
        for (i = cptr[j]; i < cptr[j + 1]; i++) {
            printf("rw[%d]\t=\t%d\n", i, rw[i]);
        }
    }
    for (j = 0; j < m; j++) { /* access by column */
        for (i = cptr[j]; i < cptr[j + 1]; i++) {
            dcy[j] += a[i] * y[j];
        }
    }

}
