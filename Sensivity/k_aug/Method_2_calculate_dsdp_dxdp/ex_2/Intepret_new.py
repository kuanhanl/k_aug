# -*- coding: utf-8 -*-
"""
Created on Thu May 21 00:09:54 2020

@author: greg6
"""

import numpy as np
import scipy.io as sio


#use dxdp-------------------------------------------------------------------
dxdp_minus = np.genfromtxt("dxdp_.dat", dtype=float)

#Solution at p=[4.5,1]
x_star = np.array([[0.5],
                  [0.5],
                  [0.0]])

#Perturb p1 from 4.5 to 4
delta_p_c1 = np.array([[-0.5],[0]]) #Note that deltaP = after - before
delta_x_c1_minus = np.matmul(dxdp_minus, delta_p_c1)
delta_x_c1 = -delta_x_c1_minus
x_est1 = x_star + delta_x_c1 #Note that deltaX = (after-b4)
# print(x_est1)
print(delta_x_c1)

#Perturb p2 from 1 to 0.8
delta_p_c2 = np.array([[0],[-0.2]]) 
delta_x_c2_minus = np.matmul(dxdp_minus, delta_p_c2)
delta_x_c2 = -delta_x_c2_minus
x_est2 = x_star + delta_x_c2 #Note that deltaX = (after-b4)
# print(x_est2)
# print(delta_x_c2)


#use dsdp(not recommend)----------------------------------------------------
dsdp_o_minus = list()
with open("dsdp_in_.in", "r") as f:
    for line in f:
        dsdp_o_minus.append(float(line))
        
dsdp_1 = []
dsdp_2 = []
for i,j in enumerate(dsdp_o_minus):
    if i < 9:
        dsdp_1.append(j)
    else:
        dsdp_2.append(j)

dsdp_minus_T = np.array([dsdp_1,
                   dsdp_2])
dsdp_minus = np.transpose(dsdp_minus_T)
dsdp = -dsdp_minus

deltaP = np.array([[-0.5], #Note that deltaP = after - before
                  [0]])
deltaS = np.matmul(dsdp,deltaP)
print(deltaS)

#use KKT matrix--------------------------------------------------------------
'''
Note that the info in kkt.in is,
1. factorized (or after pivioting)
2. symmetry
3. Only tell upper diagonal elements
4. each line in the format: row index, column index, value
'''
kkt_info = np.genfromtxt("kkt.in", dtype=float) 
row_kkt, _ = np.shape(kkt_info)  # # of total lines # in kkt.in
kkt_size = np.int(kkt_info[-1, 0])  # read the first element of last line, -1, dim of KKT matrix

KKT = np.zeros([kkt_size, kkt_size])
for ii in range(0, row_kkt):
    KKT[np.int(kkt_info[ii, 0]) - 1, np.int(kkt_info[ii, 1]) - 1] = kkt_info[ii, 2]  # row # in txt => index

def symmetrize(KKT):
    rows, cols = KKT.nonzero()  # symmetrize
    for i in range(0, len(cols)):
        KKT[cols[i], rows[i]] = KKT[rows[i], cols[i]]  # symmetrize
    return KKT
KKT = symmetrize(KKT) #This is real KKT matrix (after pivioting)

#I think KKT is after pivioting, so the right hand side, dphidp can be dealt as follows,
#(can see Yeonsoo's paper, eq 14 and the next paragraphy)
var_info = np.genfromtxt("varorder.txt", dtype=int)  # shape (*, )
const_info = np.genfromtxt("conorder.txt", dtype=int)  # shape (*, )
var_num, = np.shape(var_info)
const_num, = np.shape(const_info)
dphidp = np.zeros([var_num + const_num, 2])
p1_ind, = var_num + np.where(const_info == 1)[0]  # [0] just needed for remove []  # const 1: z(0,1) == z_ini(1)
p2_ind, = var_num + np.where(const_info == 2)[0]  # [0] just needed for remove []  # const 2: z(0,2) == z_ini(2)

dphidp[p1_ind, 0] = -1
dphidp[p2_ind, 1] = -1
deltaP = np.array([[-0.5], #Note that deltaP = after - before
                  [0]])
dphi = np.matmul(dphidp, deltaP)
ds = np.linalg.solve(KKT, -dphi)
print(ds)

