from pyomo.environ import*
from pyomo.dae import*

import numpy as np
import os

pi1 = 1.0
pi2 = 2.0
pi3 = 3.0
Ns = 10 #number of datapoints

u = range(Ns)
y_true = [pi1*i + pi2*i**2 + pi3*i**3 for i in u]

mu = 0.0
sigma = 0.1
var = 0.01
s = np.random.normal(mu, sigma, Ns)

y_meas = []
for i in u:
    meas = y_true[i] + s[i]
    y_meas.append(meas)
    
    

m = ConcreteModel()

m.k1 = Var()
m.k2 = Var()
m.k3 = Var()

m.i = Set(initialize = [i for i in range(1,Ns+1)])

ui_dict = dict()
for i in m.i:
    ui_dict[i] = i-1
m.ui = Param(m.i, initialize = ui_dict)

ymeas_dict = dict()
for i in m.i:
    # print(i)
    ymeas_dict[i] = y_meas[i-1]

m.ymeas = Param(m.i, initialize = ymeas_dict)
m.yest = Var(m.i, initialize = 10.0)

def c1(m,i):
    return m.yest[i] == m.k1*m.ui[i] + m.k2*m.ui[i]**2 + m.k3*m.ui[i]**3
m.c1 = Constraint(m.i, rule = c1)

m.obj = Objective(expr= 1/var*sum((m.yest[i] - m.ymeas[i])**2 for i in m.i))
#------------------------------------------------------------------------------    
#Suffix

#: ipopt suffixes  REQUIRED FOR K_AUG!
m.dual = Suffix(direction=Suffix.IMPORT_EXPORT)
m.ipopt_zL_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zU_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zL_in = Suffix(direction=Suffix.EXPORT)
m.ipopt_zU_in = Suffix(direction=Suffix.EXPORT)


kaug = SolverFactory('k_aug')
#: K_AUG SUFFIXES
m.dof_v = Suffix(direction=Suffix.EXPORT)  #: SUFFIX FOR K_AUG
m.rh_name = Suffix(direction=Suffix.IMPORT)  #: SUFFIX FOR K_AUG AS WELL
#: rh_name will tell us which position the corresponding variable has on the reduced hessian text file.
#: be sure to declare the suffix value (order)
m.k1.set_suffix_value(m.dof_v, 1)
m.k2.set_suffix_value(m.dof_v, 1)
m.k3.set_suffix_value(m.dof_v, 1)
kaug.options["compute_inv"] = ""  #: if the reduced hessian is desired.
#: please check the inv_.in file if the compute_inv option was used


ipopt = SolverFactory("ipopt")
ipopt.solve(m, tee = True)

m.k1.pprint()
m.k2.pprint()
m.k3.pprint()


m.ipopt_zL_in.update(m.ipopt_zL_out)
m.ipopt_zU_in.update(m.ipopt_zU_out)

#: k_aug
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


m.k1 = 5.0 #sipopt cannot start at optimal

m.red_hessian = Suffix(direction=Suffix.EXPORT)
m.k1.set_suffix_value(m.red_hessian, 1)
m.k2.set_suffix_value(m.red_hessian, 2)
m.k3.set_suffix_value(m.red_hessian, 3)

sipopt = SolverFactory('ipopt_sens')

with open('ipopt.opt', 'w') as f:
    f.write('compute_red_hessian yes\n')  #: computes the reduced hessian (sens_ipopt)
    f.write('output_file my_ouput.txt\n')
    f.write('rh_eigendecomp yes\n')
    f.close()
#: Solve
sipopt.solve(m, tee=True)
os.remove("ipopt.opt")
