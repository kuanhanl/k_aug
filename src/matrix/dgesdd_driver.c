//
// Created by dav0 on 6/14/19.
//
#define MAX(x, y) (((x) > (y)) ? (x):(y))
#define MIN(x, y) (((x) < (y)) ? (x):(y))

#include <assert.h>
#include "dgesdd_driver.h"


void dgesdd_driver(fint n, fint m, const double *a, fint nza, const fint *arow, const fint *acol) {
    /*
     * make dense matrix in column-major order
     * call dgesdd
     * */
    int i, j, k;
    int d_size = n * m;
    char job = 'N';
    double *a_dense = NULL;
    double *s = NULL;
    double *work = NULL;
    int lwork = 3 * MIN(m, n) + MAX(MAX(m, n), 7 * MIN(m, n));
    int info = 0;
    int *iwork;
    /* to be safe ... */
    lwork = lwork * 2;
    a_dense = (double *) malloc(sizeof(double) * d_size);
    assert(a_dense);
    memset(a_dense, 0, sizeof(double) * d_size);
    s = (double *) malloc(sizeof(double) * MIN(m, n));
    assert(s);
    work = (double *) malloc(sizeof(double) * lwork);
    assert(work);
    iwork = (int *) malloc(sizeof(int) * 8 * MIN(m, n));
    assert(iwork);
    for (i = 0; i < nza; i++) {
        a_dense[arow[i] + acol[i] * m] = a[i];
    }


}
