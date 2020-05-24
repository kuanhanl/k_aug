# -*- coding: utf-8 -*-
"""
Created on Thu May 14 15:30:29 2020

File from
https://github.com/dthierry/k_aug/blob/ma57/test/pyomo_/sens_kaug.py

@author: greg6
"""

#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

from pyomo.environ import *
from pyomo.opt import SolverFactory, ProblemFormat
from shutil import copyfile
"""Example taken from the sipopt manual
please check
https://github.com/coin-or/Ipopt/blob/master/Ipopt/contrib/sIPOPT/examples/redhess_ampl/red_hess.run"""

__author__ = 'David Thierry'  #: @2018

#: Declare Model
m = ConcreteModel()

m.i = Set(initialize=[1, 2, 3])

init_vals = {1:25E+07, 2:0.0, 3:0.0}
#: Variables
m.x = Var(m.i, initialize=init_vals, bounds = (0,None))
#: Objective
m.oF = Objective(expr=m.x[1]**2 +m.x[2]**2 + m.x[3]**2, 
        sense=minimize)

m.p1 = Var(initialize=4.5)
m.p2 = Var(initialize=1.0)
m.p1_0 = Param(initialize=4.5)
m.p2_0 = Param(initialize=1.0)

#: Constraints
m.c1 = Constraint(expr=6.0 * m.x[1] + 3.0 * m.x[2] + 2.0 * m.x[3] - m.p1 == 0.0)
m.c2 = Constraint(expr=m.p2 * m.x[1] + m.x[2] - m.x[3] - 1.0 == 0.0)
m.c1p = Constraint(expr=m.p1 - m.p1_0 == 0.0)
m.c2p = Constraint(expr=m.p2 - m.p2_0 == 0.0)

#: ipopt suffixes  REQUIRED FOR K_AUG!
m.dual = Suffix(direction=Suffix.IMPORT_EXPORT)
m.ipopt_zL_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zU_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zL_in = Suffix(direction=Suffix.EXPORT)
m.ipopt_zU_in = Suffix(direction=Suffix.EXPORT)


ipopt = SolverFactory('ipopt')

kaug = SolverFactory('k_aug', executable='C:\\cygwin64\\home\\greg6\\k_aug\\bin\\k_aug.exe')
dotsens = SolverFactory('dot_sens', executable='C:\\cygwin64\\home\\greg6\\k_aug\\dot_sens.exe')

#: K_AUG SUFFIXES
m.dof_v = Suffix(direction=Suffix.EXPORT)  #: SUFFIX FOR K_AUG
# m.rh_name = Suffix(direction=Suffix.IMPORT)  #: SUFFIX FOR K_AUG AS WELL
#: rh_name will tell us which position the corresponding variable has on the reduced hessian text file.
#: be sure to declare the suffix value (order)
# m.x[1].set_suffix_value(m.dof_v, 1)
#m.x[2].set_suffix_value(m.dof_v, 1)
#m.x[3].set_suffix_value(m.dof_v, 1)

#: Sensivity SUFFIXES
m.npdp = Suffix(direction=Suffix.EXPORT) #set the perturbed value of parameter, definition is p_before-p_after, in this case is 0.5 (4.5 to 4)
m.c1p.set_suffix_value(m.npdp, 0.5)
m.c2p.set_suffix_value(m.npdp, 0.0)


m.dcdp = Suffix(direction=Suffix.EXPORT) #set the order of dummy constraints
m.DeltaP = Suffix(direction=Suffix.EXPORT) #NOT set the perturbed value of parameter!!!!! Result doesn't change with different value. I guess it's just a flag.
                                            # Better keep the same value as m.npdp

m.c1p.set_suffix_value(m.dcdp, 1) 
m.c2p.set_suffix_value(m.dcdp, 2)
m.c1p.set_suffix_value(m.DeltaP, 0.5)
m.c2p.set_suffix_value(m.DeltaP, 0.0)


#: k_aug
print('k_aug \n\n\n')
# m.write('problem.nl', format=ProblemFormat.nl)

f = open('./kaug_sens.res', 'w')
f.close()

for i in range(1, 4):
    m.dof_v.clear()
    m.x[i].set_suffix_value(m.dof_v, 1)
    ipopt.solve(m)
    m.ipopt_zL_in.update(m.ipopt_zL_out)
    m.ipopt_zU_in.update(m.ipopt_zU_out)
    #: k_aug
    kaug.solve(m, tee=True)
    dotsens.solve(m, tee=True)
    with open('./dot_out.out', 'r') as f:
        lines = f.readlines()
        f.close()
    with open('./kaug_sens.res', 'a') as f:
        f.write(lines[0])
        f.close()
    copyfile('./dot_out.out', 'res_' + str(i))
    
print("over")

"""
There are a lot of output files.

sigma_warnings.txt, timings_dot_driver.txt, timings_k_aug.txt are not important.

result_red_hess.txt is the result of RH, which is not calculated in this example.

Don't know the meaning in dot_in_.in

In each iteration of for loop, dotsens calculates the seneivity result for each variable and write in dot_out.out.

The first column is x[i] with original parameter, second column is estimated result of x[i] by sensivity,
the third column is (x[i]_original - x[i]_sens_estimated).

***All the result from for loop are organized in kaug_sens.res, so just check this file for convenience.

res_i is just copy of dot_out.out at each iteration.


"""