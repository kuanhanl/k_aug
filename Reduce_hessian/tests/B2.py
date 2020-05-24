# -*- coding: utf-8 -*-
"""
Created on Tue May 12 15:43:22 2020

@author: greg6
"""


ntp = 3
nwp = 2
nc =3

i = 3
l = 1
k = 1


r_idx1 = (k-1)*ntp +i
r_idx2 = nc*ntp + (k-1)*nwp +l

c_idx = i + (l-1)*ntp

print(r_idx1, r_idx2, c_idx) 