from pyomo.environ import*
from pyomo.dae import*

m = ConcreteModel()

p1 = 4.5
p2 = 0.8

m.x1 = Var(bounds=(0,None))
m.x2 = Var(bounds=(0,None))
m.x3 = Var(bounds=(0,None))

m.p1 = Param(initialize = p1)
m.p2 = Param(initialize = p2)

def c1(m):
    return 6*m.x1 + 3*m.x2 + 2*m.x3 - p1 == 0.0

def c2(m):
    return m.p2*m.x1 + m.x2 -m.x3 -1 == 0.0

m.c1 = Constraint(rule=c1)
m.c2 = Constraint(rule=c2)

m.obj = Objective(expr=m.x1**2 + m.x2**2 + m.x3**2)

m.dual = Suffix(direction=Suffix.IMPORT_EXPORT)
m.ipopt_zL_out = Suffix(direction=Suffix.IMPORT)
m.ipopt_zU_out = Suffix(direction=Suffix.IMPORT)

ipopt = SolverFactory("ipopt")
ipopt.solve(m, tee=True)

m.pprint()