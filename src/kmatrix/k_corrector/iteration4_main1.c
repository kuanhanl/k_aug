/* @source kmatrix_b0.c
** beta 0
** April 18th, 2017
** @author: David Thierry (dmolinat@andrew.cmu) dav0@lb2016-1

********************************************************************************

@main ********************************************
**
** Reads nl file, allocates data structures, calls assembling funcs
** ToDo:
** Need to implement rhs and red hess in a single program
** Write program that takes suffixes from dot_prod calculation and performs the
** Sensitivity step. 
** @param [r] stub
** @param [r] KWs
** @@
*******************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "getstub.h"
#include "w_append_nz.h"
#include "k_assemble_cc.h"
#include "kmalloc.h"
#include "mc30_driver.h"
#include "pardiso_driver.h"

#include "get_jac_asl_aug.h"
#include "get_hess_asl_aug.h"

#include "find_inequalities.h"
#include "assemble_corrector_rhs_v2.h"
#include "assemble_rhs_rh.h"
/*#include "sens_update_driver.h"*/
#include "suffix_decl_hand.h"
#include "csr_driver.h"
#include "sigma_compute.h"
#define NUM_REG_SUF 6

static real not_zero = 1e-08;
static int dumm = 1;
static I_Known dumm_kw = {2, &dumm};
static int n_rhs = 0;
static int l_over = 0;
static I_Known l_over_kw = {1, &l_over};

static char _dot_pr_f[] = {"dot_prod"};
static char name1[] = {"smth"};
static char _n_rhsopt_[] = {"n_rhs"};
static char _no_barrieropt_[] = {"no_barrier"};
static char _no_lambdaopt_[] = {"no_lambda"};
static char _not_zero[] = {"not_zero"};



static int dot_prod_f = 0;
static I_Known dot_p_kw = {1, &dot_prod_f};

static int no_barrier = 1;
static I_Known nbarrier_kw = {0, &no_barrier};


/*static char dof_v[] = {"dof_v"};*/

/* keywords, they must be in alphabetical order! */
static keyword keywds[] = {
	KW(_dot_pr_f, IK_val, &dot_p_kw, _dot_pr_f),
  /*KW(_n_dofopt_ , I_val, &n_dof, _n_dofopt_),*/
  KW(_n_rhsopt_ , I_val, &n_rhs, _n_rhsopt_),	
  KW(_no_barrieropt_ , IK_val, &nbarrier_kw, _no_barrieropt_),
  KW(_no_lambdaopt_ , IK_val, &l_over_kw, _no_lambdaopt_),  
  KW(_not_zero , D_val, &not_zero, _not_zero),  
  KW(name1 , IK_val, &dumm_kw, name1),
};
static char banner[] = {"[KMATRIX] written by DT\n\n"};
static char _k_[] = {"K_augmented"};
static char _k_o_[] = {"K_augmented_options"};
static Option_Info Oinfo;




int main(int argc, char **argv){
	ASL *asl;
	FILE *f;

	/* SufDesc *some_suffix; */
	int i, j;
	int n_dof=0;
	int nnzw; /* let's try this */
	real *x=NULL, *lambda=NULL;
	char *s=NULL;
	SufDesc *var_f=NULL;

	SufDesc *suf_zL = NULL;
	SufDesc	*suf_zU = NULL;
	real *z_L=NULL, *z_U=NULL, *sigma=NULL;
	SufDesc *_x2 = NULL, *_lambda2 = NULL;

	SufDesc **rhs_ptr=NULL;
	SufDecl *suf_ptr=NULL;

	fint *Acol=NULL, *Arow=NULL;
	real *Aij=NULL;
	fint *Wcol=NULL, *Wrow=NULL;
	real *Wij=NULL;
	fint *Kcol=NULL, *Krow=NULL;
	real *Kij=NULL;
	fint *Kr_strt=NULL;
	fint K_nrows;

	real *S_scale=NULL;
	fint nzK;

	fint *Wc_t=NULL, *Wr_t=NULL;
	real *Wi_t=NULL;


	real *x_=NULL;
	real *dp_=NULL;
	/*real *gf= NULL; */
	real *s_star = NULL;
	int *c_flag=NULL;
	char **rhs_name=NULL;
	char **reg_suffix_name=NULL;
	int *nz_row_w=NULL;
	int *nz_row_a=NULL;
	int *md_off_w=NULL;
	int miss_nz_w;

	/* the pointer to array that will contain */
	/* the rhs */
	real *rhs_baksolve = NULL;

  FILE *somefile;
  fint nerror;
  int nzW, nzA;
  int *hr_point = NULL;
  int *positions_rh = NULL;

  char ter_msg[] = {"I[KMATRIX]...[KMATRIX_ASL]"
	"All done it seems."};

	unsigned n_r_suff = NUM_REG_SUF;
	/* Suffix names; yes, I know. */
	char _sfx_1[] = {"dof_v"};
	char _sfx_2[] = {"rh_name"};
	char _sfx_3[] = {"ipopt_zL_in"};
	char _sfx_4[] = {"ipopt_zU_in"};
	char _sfx_5[] = {"x2"};
	char _sfx_6[] = {"lambda2"};
	double *c_rh = NULL;
	printf("_nkeywds %d\n", nkeywds);


	Oinfo.sname = _k_;
	Oinfo.bsname = banner;
	Oinfo.opname = _k_o_;
	Oinfo.keywds = keywds;
	Oinfo.n_keywds = nkeywds;
	Oinfo.flags = 0;
	Oinfo.version = NULL;
	Oinfo.usage = NULL;
	Oinfo.kwf = NULL;
	Oinfo.feq = NULL;
	Oinfo.n_options = 0;
	Oinfo.driver_date = 0;
	Oinfo.wantsol = 0;
	Oinfo.nS = 0;
	Oinfo.S = NULL;
	Oinfo.uinfo = NULL;
	Oinfo.asl = NULL;
	Oinfo.eqsign = NULL;
	Oinfo.n_badopts = 0;
	Oinfo.option_echo = 0;
	Oinfo.nnl = 0;

	/* The memory allocation for asl data structure */
	asl = ASL_alloc(ASL_read_pfgh);

	s = getstops(argv, &Oinfo);

	if (!s) {
		printf("W[KMATRIX]...\t[KMATRIX_ASL]"
			"No input\n");
		return 1;
	}
	else {
		printf("I[KMATRIX]...\t[KMATRIX_ASL]"
			"File read succesfull\n");
				}

	if (n_rhs == 0){
		printf("W[KMATRIX]...\t[KMATRIX_ASL]"
			"No n_rhs declared\n");
	}
	
	if (l_over){
		printf("W[KMATRIX]...\t[KMATRIX_ASL]"
			"Multiplier check override.\n");
	}
	if (dot_prod_f){
		printf("W[KMATRIX]...\t[KMATRIX_ASL]"
			"Dot product preparation.\n");
	}


	/* Allocate suffix names (regular)*/
	reg_suffix_name =   (char **)malloc(sizeof(char *) * n_r_suff);
	for(i=0; i < (int)n_r_suff; i++){
		reg_suffix_name[i] = (char *)malloc(sizeof(char) * 16 );
		reg_suffix_name[i][0] = '\0';
	}
	
	strcat(reg_suffix_name[0], _sfx_1);
	strcat(reg_suffix_name[1], _sfx_2);
	strcat(reg_suffix_name[2], _sfx_3);
	strcat(reg_suffix_name[3], _sfx_4);
	strcat(reg_suffix_name[4], _sfx_5);
	strcat(reg_suffix_name[5], _sfx_6);

	if(n_rhs > 0){
		suf_ptr = (SufDecl *)malloc(sizeof(SufDecl)*(n_rhs + n_r_suff));
		rhs_name = (char **)malloc(sizeof(char *)*n_rhs);
		for(i=0; i<n_rhs; i++){
			rhs_name[i] = (char *)malloc(sizeof(char) * 32); /* 32 bit long digit;
			 why not?*/
		}
		suffix_decl_hand(suf_ptr, reg_suffix_name, rhs_name, n_r_suff, n_rhs);
	}
	else{
		suf_ptr = (SufDecl *)malloc(sizeof(SufDecl) * n_r_suff);
		suffix_decl_hand(suf_ptr, reg_suffix_name, rhs_name, n_r_suff, n_rhs);
	}


	printf("I[KMATRIX]...\t[KMATRIX_ASL]"
	"Number of Right hand sides %d\n", n_rhs);
	
	
	for(i=0; i<n_rhs; i++){
		printf("NAME %s\n", rhs_name[i]);
	}
	printf("nsuf %d\n", (int)n_r_suff);
	/* Declare suffixes */
	if(n_rhs > 0){suf_declare(suf_ptr, (n_rhs + n_r_suff));}
	else{suf_declare(suf_ptr, n_r_suff);}
	

	/* dhis bit setups ASL components e.g. n_var, n_con, etc. */
	f = jac0dim(s, (fint)strlen(s));


	printf("I[KMATRIX]...\t[KMATRIX_ASL]"
		"Number of Right hand sides: %d\n", n_rhs);
	printf("I[KMATRIX]...\t[KMATRIX_ASL]"
		"Number of variables       : %d\n", n_var);
	printf("I[KMATRIX]...\t[KMATRIX_ASL]"
		"Number of constraints     : %d\n", n_con);
	printf("I[KMATRIX]...\t[KMATRIX_ASL]"
		"Number of valid n_dof     : %d\n", n_var - n_con );


	if ((n_var - n_con) < 0){
		printf("E[KMATRIX]...\t[KMATRIX_ASL]"
			"nvar < ncon. This problem is not valid.\n");
		exit(-1);
	}
	
	x 		 = X0  = M1alloc(n_var*sizeof(real));
	lambda = pi0 = M1alloc(n_con*sizeof(real));
	obj_no = 0;
	/* need to do part of changing sign for y */

	pfgh_read(f, ASL_findgroups);

	
	/* NEED TO FIX THIS	*/
	if(lambda==NULL && l_over == 0){
		printf("E[KMATRIX]...\t[KMATRIX_ASL]"
	"Constraint Multipliers not declared(suffix dual), abort\n");
		for(i=0; i < (int)n_r_suff; i++){free(reg_suffix_name[i]);}
		free(reg_suffix_name);
		if(n_rhs){
			for(i=0; i<n_rhs; i++){
				free(rhs_name[i]);
			}
			free(rhs_name);
		}
		free(suf_ptr);
		ASL_free(&asl);
		exit(-1);
	}

	var_f = suf_get(reg_suffix_name[0], ASL_Sufkind_var);
	
	suf_zL = suf_get(reg_suffix_name[2], ASL_Sufkind_var| ASL_Sufkind_real); 
	suf_zU = suf_get(reg_suffix_name[3], ASL_Sufkind_var| ASL_Sufkind_real); 

	_x2 = suf_get(reg_suffix_name[4], ASL_Sufkind_var| ASL_Sufkind_real); 
	_lambda2 = suf_get(reg_suffix_name[5], ASL_Sufkind_con| ASL_Sufkind_real); 

	z_L = (real *)malloc(sizeof(real) * n_var);
	z_U = (real *)malloc(sizeof(real) * n_var);
	sigma = (real *)malloc(sizeof(real) * n_var);

	memset(z_L, 0, sizeof(real) * n_var);
	memset(z_U, 0, sizeof(real) * n_var);
	memset(sigma, 0, sizeof(real) * n_var);
	if(var_f->u.r == NULL && var_f->u.i == NULL){
    fprintf(stderr, "E[KMATRIX]...\t[KMATRIX_ASL]"
    	"suffix empty no n_dof declared!\n");
	}

	if(_x2->u.r == NULL){
    fprintf(stderr, "E[KMATRIX]...\t[KMATRIX_ASL]"
    	"suffix x2 empty!\n");
	}

	if(_lambda2->u.r == NULL){
    fprintf(stderr, "E[KMATRIX]...\t[KMATRIX_ASL]"
    	"suffix lambda2 empty!\n");
	}

	compute_sigma(asl, n_var, x, suf_zL, suf_zU, z_L, z_U, sigma, not_zero);
	
	
	/* Is this gonna work? */
	if(n_rhs > 1){
		rhs_ptr = (SufDesc **)malloc(sizeof(SufDesc *) * n_rhs);
		for(i=0; i < n_rhs; i++){
	   *(rhs_ptr + i)= suf_get(rhs_name[i], ASL_Sufkind_con|ASL_Sufkind_real);
	  	if((*(rhs_ptr + i))->u.r == NULL){
			  fprintf(stderr, "E[KMATRIX]...\t[KMATRIX_ASL]"
			  	"No rhs values declared for rhs_%d.\n", i);
			  exit(-1);
	  	}
		}
	}
	
	c_flag = (int *)malloc(sizeof(int) * n_con); /* Flags for ineq or equalities*/
	
	/*constraints flags */
	find_ineq_con(n_con, LUrhs, c_flag); /* Find the inequality constraints */

	/* Row and colum for the triplet format A matrix */
	/* size of the number of nonzeroes in the constraint jacobian */
	Acol = (fint *)malloc(sizeof(fint)*nzc);
	Arow = (fint *)malloc(sizeof(fint)*nzc);
	Aij  = (real *)malloc(sizeof(real)*nzc);


	nerror = 0;
	printf("I[KMATRIX]...\t[KMATRIX_ASL]"
		"Nonzeroes in the sparse Jacobian %d\n", nzc);

	get_jac_asl_aug (asl, x, Acol, Arow, Aij, n_var, n_con, nzc, &nerror, &nz_row_a);
	get_hess_asl_aug(asl, x, &Wcol, &Wrow, &Wij, n_var, n_con, n_obj,
	 &nnzw, lambda, &nerror, &nz_row_w, &md_off_w, &miss_nz_w);
	assert(nerror == 0);

	if(no_barrier){
	/* Add barrier term to the main diagonal */
		for(i=0; i<n_var; i++){
			j = md_off_w[i];
			Wij[j] += sigma[i];
		}
		printf("I[KMATRIX]...\t[KMATRIX_ASL]"
		"Barrier term added.\n");
	}
	

	nzA = nzc; 
	nzW = nnzw + miss_nz_w; /* Recomputed number of nz in the Hessian-of-Lagrangian */


	csr_driver((int)n_var, (int)n_con, nzW, nzA, nz_row_w, nz_row_a,
		(int*)Wrow, (int*)Wcol, Wij, (int*)Arow, (int*)Acol, Aij, 
		&Krow, &Kcol, &Kij, &Kr_strt);

	K_nrows = n_var + n_con; /* Number of rows of the KKT matrix (no ineq) */
	nzK = nzA + nzW + n_con; /* NZ in the kkt matrix (for pardiso, yes)*/
	assert(Krow != NULL);

	S_scale = (real *)calloc(sizeof(real), K_nrows);
	mc30driver(K_nrows, nzK, Kij, Krow, Kcol, S_scale);

	/* */
  /* by [rhs][n] */
   

  s_star			 = (real *)calloc(K_nrows, sizeof(real));
   /* array that contains the primal dual update */

  /* Primal-dual vector */
  for(i=0; i<K_nrows; i++){
  	if(i<n_var){
  		s_star[i] = x[i];
  	}
  	else{
  		s_star[i] = lambda[i - n_var];
  	}
  }
  
  somefile = fopen("primal_dual.txt", "w");
  
  for(i=0; i<K_nrows; i++){
    fprintf(somefile, "\t%f\n", s_star[i]);
  }
  fclose(somefile);

  if(_x2->u.r){
  	for(i=0; i<n_var; i++){
  		x[i] = _x2->u.r[i];
  	}
    fprintf(stderr, "E[KMATRIX]...\t[KMATRIX_ASL]"
    	"updated primal variables!\n");
	}

	if(_lambda2->u.r){
		for(i=0; i<n_con; i++){
  		lambda[i] = _lambda2->u.r[i];
  	}
    fprintf(stderr, "E[KMATRIX]...\t[KMATRIX_ASL]"
    	"updated dual variables!\n");
	}

	assemble_corrector_rhs_v2(asl, x, lambda, n_var, n_con,Arow,Acol,Aij,nzA,&c_rh,&nerror, c_flag);
  

  x_           = (real *)calloc(K_nrows, sizeof(real));
  
  /* scale matrix */
  for(i=0; i< nzK; i++){
  	Kij[i] = Kij[i] * exp(S_scale[Kcol[i]-1]) * exp(S_scale[Krow[i]-1]);
  }
  
  
  /* v3 */
  for(j=0; j < K_nrows; j++){
  	c_rh[j] = c_rh[j] * exp(S_scale[j]);
  }
  
	somefile = fopen("rhs_corrector_scaled.txt", "w");

 	/* v3 */
 	for(j=0; j < K_nrows; j++){
 		fprintf(somefile, "\t%.g\t", c_rh[j] );
 	}
 	fclose(somefile);

 
  /* factorize the matrix */
	pardiso_driver(Kr_strt, Kcol, Kij, K_nrows, nzK, 1, c_rh, x_);
      
  printf("I[KMATRIX]...\t[KMATRIX_ASL]"
		"Pardiso done. \n");

  somefile = fopen("result_scaled.txt", "w");
  for(i=0; i<K_nrows; i++){
    fprintf(somefile, "\t%.g", x_[i]);
  }
  fclose(somefile);

  somefile = fopen("result_unscaled.txt", "w");
  for(i=0; i<K_nrows; i++){
		x_[i] = x_[i] * exp(S_scale[i]);
    fprintf(somefile, "%.g\n", x_[i]);
  }
  fclose(somefile);



  somefile = fopen("primal_dual.txt", "w");
  
  for(i=0; i<K_nrows; i++){
    fprintf(somefile, "\t%f\n", s_star[i]);
  }
  fclose(somefile);

  suf_rput(reg_suffix_name[4], ASL_Sufkind_var | ASL_Sufkind_real, x_);
  suf_rput(reg_suffix_name[5], ASL_Sufkind_con | ASL_Sufkind_real, x_ + n_var);
  
  solve_result_num = 0;
  write_sol(ter_msg, s_star, s_star + n_var, &Oinfo);

  for(i=0; i<(int)n_r_suff; i++){
  	free(reg_suffix_name[i]);
  }
  free(c_rh);
  free(sigma);
  free(z_L);
	free(z_U);
	free(reg_suffix_name);
	free(nz_row_w);
	free(nz_row_a);
	free(md_off_w);
	free(rhs_baksolve);
	free(x_);
	free(hr_point);
	free(dp_);
	free(s_star);
	/* suf_name = (char **)malloc(sizeof(char *)*n_rhs); */
	free(S_scale);
	free(c_flag);

	

	for(i=0; i<n_rhs; i++){
		free(rhs_name[i]);
	}
	free(rhs_name);

	ASL_free(&asl);
	free(suf_ptr);
	free(Arow);
	free(Acol);
	free(Aij);
	free(Wrow);
	free(Wcol);
	free(Wij);
	free(Wr_t);
	free(Wc_t);
	free(Wi_t);
	free(Krow);
	free(Kcol);
	free(Kij);
	free(Kr_strt);
	free(rhs_ptr);
	return 0;
}