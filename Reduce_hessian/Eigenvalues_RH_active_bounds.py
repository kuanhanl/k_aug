#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

from pyomo.environ import *
from pyomo.opt import SolverFactory, ProblemFormat

"""Example taken from the sipopt manual
please check
https://github.com/coin-or/Ipopt/blob/master/Ipopt/contrib/sIPOPT/examples/redhess_ampl/red_hess.run"""

__author__ = 'David Thierry'  #: @2018

#: Declare Model---------------------------------------------------------------
m = ConcreteModel()

m.i = Set(initialize=[1, 2, 3])

init_vals = {1:25E+07, 2:0.0, 3:0.0}
#: Variables
# m.x = Var(m.i, initialize=init_vals)
m.x = Var(m.i, initialize=init_vals, bounds=(0, None))
#: Objective
m.oF = Objective(expr=(m.x[1])**2 + (m.x[2] - 2.0)**2 + (m.x[3] - 3.0)**2, sense=minimize)
m.oF.pprint()
#: Constraints
m.c1 = Constraint(expr=m.x[1] - 3 * m.x[2] + 2*m.x[3] == 0.0)
#------------------------------------------------------------------------------


#: ipopt suffixes  REQUIRED FOR K_AUG!-----------------------------------------
m.dual = Suffix(direction=Suffix.IMPORT_EXPORT)
m.ipopt_zL_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zU_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zL_in = Suffix(direction=Suffix.EXPORT)
m.ipopt_zU_in = Suffix(direction=Suffix.EXPORT)
#------------------------------------------------------------------------------

#: define solvers--------------------------------------------------------------
ipopt = SolverFactory('ipopt')
kaug = SolverFactory('k_aug')
#------------------------------------------------------------------------------


#: K_AUG SUFFIXES--------------------------------------------------------------
m.dof_v = Suffix(direction=Suffix.EXPORT)  #: SUFFIX FOR K_AUG
m.rh_name = Suffix(direction=Suffix.IMPORT)  #: SUFFIX FOR K_AUG AS WELL
#: rh_name will tell us which position the corresponding variable has on the reduced hessian text file.
#: be sure to declare the suffix value (order)
#m.x[1].set_suffix_value(m.dof_v, 1)
m.x[2].set_suffix_value(m.dof_v, 1)
m.x[3].set_suffix_value(m.dof_v, 1)
kaug.options["compute_inv"] = ""  #: if the reduced hessian is desired.
#: please check the inv_.in file if the compute_inv option was used
#------------------------------------------------------------------------------


ipopt.solve(m, tee=True)

m.x.display()
m.ipopt_zL_out.pprint()

m.ipopt_zL_in.update(m.ipopt_zL_out)
m.ipopt_zU_in.update(m.ipopt_zU_out)


# : k_aug
print('k_aug \n\n\n')
#m.write('problem.nl', format=ProblemFormat.nl)
kaug.solve(m, tee=True)
print('k_aug red_hess')
with open('result_red_hess.txt', 'r') as f:
    lines = f.readlines()
    for i in lines:
        print(i)
    f.close()

m.rh_name.pprint() #: in this case the positions in the matrix are given by rh_name.
#: note that the indices start from 0!! (asl does not return 0, so one of the dof_v vars will be missing.