/*
  Created by dav0 on 4/24/18.
*/

#ifndef K_AUG_DATA_H
#define K_AUG_DATA_H

typedef struct nlp_info{
    int n_orig;
    int m_orig;
    int m_eq;
    int m_ineq;
    int m_gl;
    int m_gu;
    int m_glu;
    int n_slack;
    int *con_slack; /* constraint number for a given slack j */
    int *slack_con; /* slack number for a given constraint j */
    int len_active_bnd;
    int *eq_c;
    int *gl_c;
    int *gu_c;
    int *glu_c;
    int *con_flag;
    int *nz_J;
    void (*nlp_id)(struct nlp_info *); /* deallocate function pointer */
} nlp_info;

typedef struct nlp_pd {
    double *x; /* trial */
    double *yu, *yl, *y;
    double *zu, *zl, *z;
    double *x_current;
    double *y_current;
    double *slack_curr;
    double *x_orig; /* no slack */
    double *y_orig;
    double *x_0;
    double *y_0;
    double *zl_0;
    double *zu_0;
    double *s0;
    double *grad_f;
    double *jac_c;
    double *grad_L;
    double *grad_c_y;
    int *jc_r;
    int *jcptr;
    double *hess_l;
    int *hl_r;
    int *hlptr;

    void (*nlp_pdd)(struct nlp_pd *); /* deallocator function pointer */
} nlp_pd;


typedef struct inertia_strategy_params{
    double km;
    double kp;
    double kbp;
    double kc;
    double dcb;
    double d_w0;
    double dmin;
    double dmax;
} inertia_params;

typedef struct inertia_strategy_options{
    int no_inertia;
    int always_perturb_jacobian;
    int jacobian_perturbed;
} inertia_options;

typedef struct inertia_perturbations{
    double d_w;
    double d_c;
    double d_w_last;
    double d_c_last;
    int jacobian_perturbed;
} inertia_perts;

typedef struct linear_solver_options{
    int want_accurate;
    int max_refinement_steps;
    int max_inertia_steps;
    int ls_internal_scaling;
    double residual_ratio_max;
    int max_memory_al_steps;
    double pivot_tol0;
    double pivtol_max;
} linsol_opts;


void nlpi_deallocate(nlp_info *nlp_info1);

void nlppd_deallocate(nlp_pd *nlp_pd1);

#endif /*K_AUG_DATA_H*/
