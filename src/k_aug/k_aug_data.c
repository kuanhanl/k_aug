//
// Created by dav0 on 8/24/18.
//
#include <stdlib.h>
#include <malloc.h>
#include "k_aug_data.h"
extern int make_iso_compilers_happy;


void nlpi_deallocate(nlp_info *nlp_info1) {
    free(nlp_info1->con_slack);
    free(nlp_info1->slack_con);
    free(nlp_info1->eq_c);
    free(nlp_info1->gl_c);
    free(nlp_info1->gu_c);
    free(nlp_info1->glu_c);
    free(nlp_info1->con_flag);
    printf("nlp_info1->nz_J\t%p\n", (void *) nlp_info1->nz_J);
    free(nlp_info1->nz_J);
    printf("Все сделано!\n");
}


void nlppd_deallocate(nlp_pd *nlp_pd1) {
    free(nlp_pd1->x_0);
    free(nlp_pd1->y_0);
    free(nlp_pd1->x_orig);
    free(nlp_pd1->y_orig);
    free(nlp_pd1->slack_curr);
    free(nlp_pd1->jac_c);
    free(nlp_pd1->jc_r);
    free(nlp_pd1->jcptr);
    free(nlp_pd1->grad_f);
    free(nlp_pd1->hess_l);
    free(nlp_pd1->hl_r);
    free(nlp_pd1->hlptr);
    free(nlp_pd1->y);
    free(nlp_pd1->yl);
    free(nlp_pd1->yu);

    free(nlp_pd1->z);
    free(nlp_pd1->zl);
    free(nlp_pd1->zu);
    free(nlp_pd1->grad_c_y);
    free(nlp_pd1->grad_L);

    printf("Все сделано!\n");
}
