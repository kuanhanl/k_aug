//
// Created by dav0 on 9/12/18.
//

#include "slacked_grad.h"

void slacked_grad(ASL *asl, nlp_info *nlp_i, nlp_pd *nlp_pd) {
    int i = 0, n;
    double *g = NULL;
    FILE *myf = NULL;
    n = nlp_i->n_orig + nlp_i->n_slack;
    g = (double *) malloc(sizeof(double) * n);
    memset(g, 0, sizeof(double) * n);
    asl->p.Objgrd(asl, 0, nlp_pd->x_0, g, &i);
    nlp_pd->grad_f = g;

    myf = fopen("grad_f", "w");
    for (i = 0; i < n; i++) { fprintf(myf, "%f\n", g[i]); }
    fclose(myf);
}
