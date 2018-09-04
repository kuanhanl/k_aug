/* @source find_inequalities.c
** 1.1
** August 2018
** @author: David Thierry (dmolinat@andrew.cmu) dav0@lb2016-1

********************************************************************************

@find_ineq_con ********************************************
**
** Finds the nature of the constraints involved in the problem.
** Description
**
** @param [r]
** @param [r]
** @param [r]
** @param [r]
** @param [r]
** @param [r]
** @param [r]
** @return something
*******************************************************************************/

#include "con_check.h"


void con_check(fint ncon_, real *LBC, nlp_info *nlp_i) {
    int i, j, s = 0;
    int geq_con, leq_con, eq_con, n_double;
    geq_con = 0;
    leq_con = 0;
    eq_con = 0;
    n_double = 0;
    int *si = NULL;

    si = (int *) malloc(sizeof(int) * nlp_i->m_orig);

    /* Inequality cases for general form gl <= g(x) <= gu:
     * 299  gl < -inf, and gu <= inf (ERROR)
     * 3    gl = gu (Equalitity)
     * -1   gl <= g(x) <= gu (General inequality)
     * 1    gl <= -inf --> g(x)<=gu (inequality lower or equal gu)
     * 2    gu >= -inf --> g(x)>=gl (inequality greater or equal gl) */
    for (i = 0; i < ncon_; i++) {
        if (LBC[2 * i] <= -1e300 && LBC[2 * i + 1] >= 1e300) {
            /* Invalid case */
            printf("I[K_AUG]...\t[FIND_INEQ]"
                   "-inf <= c(x) <= +inf detected\b");
            nlp_i->con_flag[i] = 299;
            fprintf(stderr, "Unexpected behavior. \t[FIND_INEQ]");
        } else if (LBC[2 * i] > -1e300 && LBC[2 * i + 1] < 1e300) {
            if (LBC[2 * i + 1] < LBC[2 * i]) {
                /* Invalid case */
                fprintf(stderr, "Invalid case found. \t[FIND_INEQ]\n");
                fprintf(stderr, "Constraint %d\tgl = %.g\tgu = %.g\n", i, LBC[2 * i], LBC[2 * i + 1]);
                exit(-1);
            } else if (LBC[2 * i + 1] == LBC[2 * i]) {
                nlp_i->eq_c[eq_con] = i;
                eq_con++;
                nlp_i->con_flag[i] = 3;

                /*if(LBC[2*i]!=0.0){nlp_i->con_flag[i] = -3;}*/
            } else {
                /* proper gl <= g(x) <= gu i.e. two inequalities */
                nlp_i->glu_c[n_double] = i;
                n_double++;
                nlp_i->con_flag[i] = -1;
                si[s] = i;
                s++;
                /*si[s] = i;
                s++;*/
            }
        } else if (LBC[2 * i] <= -1e300) {
            nlp_i->gl_c[leq_con] = i;
            leq_con++;
            nlp_i->con_flag[i] = 1;
            si[s] = i;
            s++;
            /*if(LBC[2*i+1]!=0.0){nlp_i->con_flag[i] = -1;}*/
        } else if (LBC[2 * i + 1] >= 1e300) {
            nlp_i->gu_c[geq_con] = i;
            geq_con++;
            nlp_i->con_flag[i] = 2;
            si[s] = i;
            s++;
            /*if(LBC[2*i]!=0.0){nlp_i->con_flag[i] = -2;}*/
        } else {
            /* Invalid case */
            fprintf(stderr, "Invalid case found. \t[FIND_INEQ]\n");
            fprintf(stderr, "Constraint %d\tgl = %.g\tgu = %.g\n", i, LBC[2 * i], LBC[2 * i + 1]);
            exit(-1);
        }
    }

    if (n_double > 0) {
        printf("I[K_AUG]...\t[FIND_INEQ]"
               "Summary: eq: %d\t, leq: %d\t, geq: %d\t, *double ineq %d\n", eq_con, leq_con, geq_con, n_double);
    } else {
        printf("I[K_AUG]...\t[FIND_INEQ]"
               "Summary: eq: %d\t, leq: %d\t, geq: %d\n", eq_con, leq_con, geq_con);
    }
    /* nlp_i->slack_i = (int *)malloc(sizeof(int) * (s + n_double)); */
    nlp_i->slack_i = (int *) malloc(sizeof(int) * (s + n_double));
    for (i = 0; i < s; i++) {
        nlp_i->slack_i[i] = si[i];
    }

    for (i = 0; i < n_double; i++) {
        j = nlp_i->glu_c[i];
        nlp_i->slack_i[s + i] = j;
    }

    for (i = 0; i < (s + n_double); i++) {
        printf("slack %d\t%d\n", i, nlp_i->slack_i[i]);
    }
    free(si);
    nlp_i->m_eq = eq_con;
    nlp_i->m_gl = leq_con;
    nlp_i->m_gu = geq_con;
    nlp_i->m_glu = n_double;
}


void find_bounds(fint nvar_, real *lbv) {
    int i;

    for (i = 0; i < nvar_; i++) {
        printf("%d LBC %f \t UBC %f\n", i, lbv[2 * i], lbv[2 * i + 1]);
    }
}
