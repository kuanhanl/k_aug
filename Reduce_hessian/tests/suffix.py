# -*- coding: utf-8 -*-
"""
Created on Thu May  7 16:00:06 2020

@author: greg6
"""

from pyomo.environ import *

model = ConcreteModel()
model.x = Var()
model.y = Var([1,2,3])
model.foo = Suffix()

model.foo.set_value(model.x, 1.0)