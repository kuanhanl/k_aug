/* @source mumps_driver.h
**
** January 11th, 2018
** @author: David Thierry (dmolinat@andrew.cmu) dav0@lb2016-1

******************************************************************************

@pmumps_driver ********************************************
**
** interface to mumps, solves linear system
**
** @param [r] ia, row starts
** @param [r] ja, sparse column
** @param [r] a, sparse matrix element
** @param [r] n, number of rows
** @param [r] nza, number of nz of a
** @param [r] nrhs, number of right hand sides
** @param [r] b, right hand side vector(matrix)
** @param [r] x, solution vector

*******************************************************************************/

#ifndef MUMPS_DRIVER
#define MUMPS_DRIVER

#include "../../../thirdparty/mumps512/MUMPS_5.1.2/include/dmumps_c.h" /* header that contains types and mumps structure */
#include "../../ASL/solvers/asl.h"
#include "../../k_aug/k_aug_data.h"

int mumps_driver(fint *ia, fint *ja, double *a, fint n, int n_rhs, double *b, double *x, int nvar, int ncon, int no_inertia,
                 int nza, inertia_perts *i_pert, inertia_params i_parm,
                 inertia_options i_opts, double log10mu, fint *row_starts);

#endif /* MUMPS_DRIVER */