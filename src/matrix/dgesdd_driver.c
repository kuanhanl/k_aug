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
    FILE *whatevs;
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
    /*
     * fortran row and column indexing, starting at 1 */
    /*
    * whatevs = fopen("sparse_.txt", "w");
    * for (i = 0; i < nza; i++) {fprintf(whatevs,"a[%d, %d] = %f\n", arow[i], acol[i], a[i]);}
    * fclose(whatevs);
     */

    for (i = 0; i < nza; i++) {
        a_dense[(arow[i] - 1) + (acol[i] - 1) * m] = a[i];
    }
    /*
    * whatevs = fopen("dense_.txt", "w");
    * for(i = 0; i<n; i++){
    *    for(j = 0; j<m; j++){
    *        fprintf(whatevs, "a[%d, %d] = %f\n", i, j, a_dense[i + j * m]);
    *    }
    }
     */

    dgesdd_(&job, &m, &n, a_dense, &m, s, NULL, &m, NULL, &m, work, &lwork, iwork, &info);

    printf("info %d\n\n", info);
    if (info == 0){
        whatevs = fopen("singular_values_.txt", "w");
        for(i = 0; i < MIN(m,n); i++){
            fprintf(whatevs, "%.g\n", s[i]);
        }
        fclose(whatevs);
    }

    free(a_dense);
    free(s);
    free(work);
    free(iwork);
}
