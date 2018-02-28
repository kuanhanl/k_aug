CC = gcc
#CCFLAGS = -g -Wall -ansi -pedantic-errors -Wall -W -Wconversion -Wshadow -Wcast-qual -Wwrite-strings
CCFLAGS = -g -Wall -ansi -pedantic-errors -Wall -W
FC = gfortran
S = ..
L = -lm -ldl
A = $S/ASL/solvers/amplsolver.a
ONAME = k_aug


all1 = k_aug
MATRIXDIR = ../matrix
LMATRIX = kassemble.f

MC30ROOT = /home/dav0/Apps/mc30-1.0.0/build/src/


PARDISOROOT = /usr/local/lib/


OPENBLASROOT = /home/dav0/Apps/OpenBLAS/
BLASROOT = $(S)/BLAS/BLAS-3.7.0/

LAPACKROOT = $(S)/LAPACK/lapack-3.8.0/


LMKLIB= -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl   -m64 -I${MKLROOT}/include
LIBPARDISO = -L${PARDISOROOT} -lpardiso500-GNU481-X86-64 -lgfortran -fopenmp -lpthread -lm
# example LIBOPENBLAS= -L${OPENBLASROOT} -lopenblas_haswellp-r0.2.20.dev
LIBOPENBLAS= -L${OPENBLASROOT} -lopenblas_haswellp-r0.2.20.dev
LIBBLAS = $(BLASROOT)blas_LINUX.a
LIBLAPACK = $(LAPACKROOT)liblapack.a

LIBMC30 = ${MC30ROOT}libmc30.a -lgfortran -lm -lquadmath
KMATRIX = iteration4_main1.c

k_aug_pard: mc30_dri \
      get_jac_asl get_hess_asl find_ineq get_grad_f assemble_rhsds_rh assemble_rhsds_dcdp \
      suffix_decl_hand csr_driver pard_dri sig_comp mu_ad dsyev_eig dpotri_driver
	$(CC) $(CCFLAGS) $(KMATRIX) -o $(ONAME)  $A $L \
	mc30_driver.o  \
	get_jac_asl_aug.o get_hess_asl_aug.o find_inequalities.o get_grad_f.o \
	assemble_rhs_rh.o assemble_rhs_dcdp.o  \
	suffix_decl_hand.o csr_driver.o pardiso_driver.o \
	sigma_compute.o mu_adjust_primal.o dsyev_driver.o dpotri_driver.o \
	-I$(S)/HSL/ -I$(S)/Pardiso/ -I$(S)/matrix/ -I$(S)/ASL/solvers/\
	 $(LIBMC30) $(LIBPARDISO) $(LIBLAPACK) $(LIBBLAS)

k_aug_ma57: mc30_dri \
      get_jac_asl get_hess_asl find_ineq get_grad_f assemble_rhsds_rh \
      suffix_decl_hand csr_driver sig_comp mu_ad dsyev_eig  ma57_driver
	$(CC) $(CCFLAGS) iteration4_main_ma_57.c -o k_aug_m  $A $L \
	mc30_driver.o  ma57_driver.o \
	get_jac_asl_aug.o get_hess_asl_aug.o find_inequalities.o get_grad_f.o \
	assemble_rhs_rh.o  \
	suffix_decl_hand.o csr_driver.o  \
	sigma_compute.o mu_adjust_primal.o dsyev_driver.o \
	-I$(S)/HSL/ -I$(S)/matrix/ -I$(S)/ASL/solvers/\
	 $(LIBMC30) -L/usr/local/lib/ -lcoinhsl -lmetis  $(LIBLAPACK) $(LIBBLAS)


clean:
	rm -rf $(all) *.o
      
mc30_dri:
	$(CC) $(CCFLAGS) -c $(S)/HSL/mc30_driver.c -I$(S)/HSL/ -I$S/ASL/solvers/

pard_dri:
	$(CC) $(CCFLAGS) -c $(S)/Pardiso/pardiso_driver.c -I$(S)/ASL/solvers/ 

get_jac_asl:
	$(CC) $(CCFLAGS) -c get_jac_asl_aug.c -I$S/ASL/solvers/

get_hess_asl:
	$(CC) $(CCFLAGS) -c get_hess_asl_aug.c -I$S/ASL/solvers/

find_ineq:
	$(CC) $(CCFLAGS) -c find_inequalities.c -I$S/ASL/solvers/

get_grad_f:
	$(CC) $(CCFLAGS) -c get_grad_f.c -I$S/ASL/solvers/

assemble_corrector_rhs:
	$(CC) $(CCFLAGS) -c assemble_corrector_rhs.c -I$S/ASL/solvers/

sens_update_driver:
	$(CC) $(CCFLAGS) -c $(S)/matrix/sens_update_driver.c -I$S/ASL/solvers/

assemble_rhsds_rh:
	$(CC) $(CCFLAGS) -c assemble_rhs_rh.c -I$(S)/ASL/solvers/

assemble_rhsds_dcdp:
	$(CC) $(CCFLAGS) -c assemble_rhs_dcdp.c -I$(S)/ASL/solvers/

suffix_decl_hand:
	$(CC) $(CCFLAGS) -c suffix_decl_hand.c -I$(S)/ASL/solvers/

csr_driver:
	$(CC) $(CCFLAGS) -c csr_driver.c -I$(S)/ASL/solvers/

sig_comp:
	$(CC) $(CCFLAGS) -c sigma_compute.c -I$(S)/ASL/solvers/

mu_ad:
	$(CC) $(CCFLAGS) -c mu_adjust_primal.c -I$(S)/ASL/solvers/

dsyev_eig:
	$(CC) $(CCFLAGS) -c $(S)/matrix/dsyev_driver.c -I$(S)/ASL/solvers/

ma57_driver:
	$(CC) $(CCFLAGS) -c $(S)/HSL/ma57_driver.c 
	#-L/usr/local/lib/ -lcoinhsl -lmetis $(LIBBLAS) $(LIBLAPACK) -lgfortran -lm -ldl

dpotri_driver:
	$(CC) $(CCFLAGS) -c $(S)/matrix/dpotri_driver.c 