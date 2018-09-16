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
}
