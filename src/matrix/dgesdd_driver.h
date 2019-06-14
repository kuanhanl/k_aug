//
// Created by dav0 on 6/14/19.
//

#ifndef K_AUG_DGESDD_DRIVER_H
#define K_AUG_DGESDD_DRIVER_H

#include "../../thirdparty/asl/solvers/asl.h"

void dgesdd_(char *jobz, int *m, int *n, double *a, int *lda, double *s, double *u, int *ldu, double *vt, int *ldvt,
             double *work, int *lwork, int *iwork, int *info);
void dgesdd_driver(fint n, fint m, const double *a, fint nza, const const fint *arow, const fint *acol);

/*
 * character  	JOBZ,
 *  integer  	M,
 *  integer  	N,
 *  double precision, dimension( lda, * )  	A,
 *  integer  	LDA,
 *  double precision, dimension( * )  	S,
 *  double precision, dimension( ldu, * )  	U,
 *  integer  	LDU,
 *  double precision, dimension( ldvt, * )  	VT,
 *  integer  	LDVT,
 *  double precision, dimension( * )  	WORK,
 *  integer  	LWORK,
 *  integer, dimension( * )  	IWORK,
 *  integer  	INFO */

#endif //K_AUG_DGESDD_DRIVER_H
