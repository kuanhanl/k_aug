# -*- coding: utf-8 -*-
"""
Created on Thu May  7 13:57:11 2020

@author: greg6
"""

from pyomo.environ import *

model = ConcreteModel()
model.dual = Suffix(direction=Suffix.IMPORT)
model.x = Var()
model.obj = Objective(expr=model.x)
model.con = Constraint(expr=model.x>=1.0)

SolverFactory('ipopt').solve(model,tee = True)
#print(model.dual[model.con])