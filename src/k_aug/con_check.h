#ifndef FIND_INEQ_CON
#define FIND_INEQ_CON

#include "../../thirdparty/asl/solvers/asl.h"
#include "k_aug_data.h"

void con_check(ASL *asl, nlp_info *nlp_i);
void find_bounds  (fint nvar_,real *lbv);



#endif