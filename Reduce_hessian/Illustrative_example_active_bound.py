from pyomo.environ import*
from pyomo.dae import*

m = ConcreteModel()

# m.x1 = Var(initialize = 0.1)
# m.x2 = Var(initialize = 0.0)
# m.x3 = Var(initialize = 0.0)

m.x1 = Var(initialize = 0.1, bounds = (1, None))
m.x2 = Var(initialize = 0.0, bounds = (0, None))
m.x3 = Var(initialize = 0.0, bounds = (0, None))

def c1(m):
    return -7*m.x1 + 2*m.x2 + m.x3 == 0.0
m.c1 = Constraint(rule = c1)

def obj(m):
    return (m.x1-1.0)**2 + (m.x2-2.0)**2 + (m.x3-3.0)**2
m.obj = Objective(rule = obj)


m.dual = Suffix(direction=Suffix.IMPORT_EXPORT)
m.ipopt_zL_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zU_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zL_in = Suffix(direction=Suffix.EXPORT)
m.ipopt_zU_in = Suffix(direction=Suffix.EXPORT)


ipopt = SolverFactory('ipopt')
kaug = SolverFactory('k_aug')

m.dof_v = Suffix(direction=Suffix.EXPORT)  #: SUFFIX FOR K_AUG
m.rh_name = Suffix(direction=Suffix.IMPORT)  #: SUFFIX FOR K_AUG AS WELL
#: rh_name will tell us which position the corresponding variable has on the reduced hessian text file.
#: be sure to declare the suffix value (order)
#m.x[1].set_suffix_value(m.dof_v, 1)
m.x2.set_suffix_value(m.dof_v, 1)
m.x3.set_suffix_value(m.dof_v, 1)
kaug.options["compute_inv"] = "" 


ipopt.solve(m, tee=True)

m.ipopt_zL_in.update(m.ipopt_zL_out)
m.ipopt_zU_in.update(m.ipopt_zU_out)

print('k_aug \n\n\n')
#m.write('problem.nl', format=ProblemFormat.nl)
kaug.solve(m, tee=True)
print('k_aug red_hess')
with open('result_red_hess.txt', 'r') as f:
    lines = f.readlines()
    for i in lines:
        print(i)
    f.close()

m.rh_name.pprint()

m.pprint()