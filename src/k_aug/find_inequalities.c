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

#include "find_inequalities.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <assert.h>

void find_ineq_con(fint ncon_,real *LBC, int *c_flag, int *ngl, int *ngu){
	int i;
	int geq_con, leq_con, eq_con;
	geq_con=0;
	leq_con=0;
	eq_con =0;
	for(i=0; i<ncon_; i++){
        if (LBC[2*i] <= -1e300 && LBC[2*i+1] >= 1e300){
            /* This will never happen? */
            printf("I[K_AUG]...\t[FIND_INEQUALITIES]"
                   "-inf <= c(x) <= +inf detected\b");
            c_flag[i] = 299;
            fprintf(stderr, "Unexpected behavior. \t[FIND_INEQUALITIES]");
        }
	    else if(LBC[2*i] <= -1e300){
			leq_con++;
			c_flag[i] = 1;
			if(LBC[2*i+1]!=0.0){c_flag[i] = -1;}
		}
		else if(LBC[2*i+1] >= 1e300){
			geq_con++;
			c_flag[i] = 2;
			if(LBC[2*i]!=0.0){c_flag[i] = -2;}
		}

		else{
			eq_con++;
			c_flag[i] = 3;
			if(LBC[2*i]!=0.0){c_flag[i] = -3;}
		}
	}
	printf("I[K_AUG]...\t[FIND_INEQUALITIES]"
        "Summary: eq: %d, leq: %d, geq: %d \n", eq_con, leq_con, geq_con);
	*ngl = leq_con;
	*ngu = geq_con;
}



void find_bounds(fint nvar_,real *lbv){
	int i;

	for(i=0; i<nvar_; i++){
		printf("%d LBC %f \t UBC %f\n",i, lbv[2*i], lbv[2*i+1]);
	}
}
