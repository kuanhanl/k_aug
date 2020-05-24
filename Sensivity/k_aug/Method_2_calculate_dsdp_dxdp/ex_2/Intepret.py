import numpy as np
from scipy.sparse import lil_matrix
import scipy.io as sio

dxdp = np.loadtxt('dxdp_.dat', usecols=range(2))

x_star = np.array([[0.5],
                  [0.5],
                  [0.0]])

# Perturb the first parameter from 4.5 to 4
deltaP1 = np.array([[0.5],
                  [0]])
deltax1 = dxdp.dot(deltaP1)
x_est1 = x_star + deltax1
print(x_est1)

# Perturb the second parameter from 1 to 0.8
deltaP2 = np.array([[0.0],
                  [0.2]])
deltax2 = dxdp.dot(deltaP2)
x_est2 = x_star + deltax2
print(x_est2)



# print("\nusing dsdp")
dsdp_o = list()
with open("dsdp_in_.in", "r") as f:
    for line in f:
        dsdp_o.append(float(line))
        
dsdp_1 = []
dsdp_2 = []
for i,j in enumerate(dsdp_o):
    if i < 9:
        dsdp_1.append(j)
    else:
        dsdp_2.append(j)

dsdp_T = np.array([dsdp_1,
                   dsdp_2])

dsdp = np.transpose(dsdp_T)

deltaP = np.array([[-0.5],
                  [0]])
deltas = dsdp.dot(deltaP)
# print(deltas)

sp4 = np.array([[0.333],
                [1],
                [0.666],
                [0],
                [4],
                [-2],
                [0.22],
                [0.22],
                [0.83]])

sp45 = np.array([[0.5],
                [1],
                [0.5],
                [0],
                [4.5],
                [-1],
                [0],
                [0],
                [0.5]])

deltas_check = np.subtract(sp45,sp4)
# print("\n",deltas_check)

def symmetrize(MAT):
    rows, cols = MAT.nonzero()  # symmetrize
    for i in range(0, len(cols)):
        MAT[cols[i], rows[i]] = MAT[rows[i], cols[i]]  # symmetrize
    return MAT


kkt_info = np.genfromtxt("kkt.in", dtype=float)
row_kkt, _ = np.shape(kkt_info)  # # of total lines # in kkt.in
kkt_size = np.int(kkt_info[-1, 0])  # read the first element of last line, -1, dim of MAT

MAT = np.zeros([kkt_size, kkt_size])
for ii in range(0, row_kkt):
    MAT[np.int(kkt_info[ii, 0]) - 1, np.int(kkt_info[ii, 1]) - 1] = kkt_info[ii, 2]  # row # in txt => index
MAT = symmetrize(MAT)

var_info = np.genfromtxt("varorder.txt", dtype=int)  # shape (*, )
const_info = np.genfromtxt("conorder.txt", dtype=int)  # shape (*, )
var_num, = np.shape(var_info)
const_num, = np.shape(const_info)
dphidp = np.zeros([var_num + const_num, 2])
p1_ind, = var_num + np.where(const_info == 1)[0]  # [0] just needed for remove []  # const 1: z(0,1) == z_ini(1)
p2_ind, = var_num + np.where(const_info == 2)[0]  # [0] just needed for remove []  # const 2: z(0,2) == z_ini(2)

dphidp[p1_ind, 0] = -1
dphidp[p2_ind, 1] = -1
dphi = np.matmul(dphidp, deltaP)
ds = np.linalg.solve(MAT, -dphi)

# dphidp_T = np.array([[0,0,0,0,0,0,-1,1,0],
#                      [-1,0,0,0,0,0.5,0,0,1]])
# dphidp_T = np.array([[0,0,0,0,-1,0,0,0,0],
#                      [0,-1,0,0,0,0,0,0,0]])
# dphidp = np.transpose(dphidp_T)

# dsdp_cal = np.linalg.solve(MAT.toarray(), -dphidp)

# ds_cal = np.linalg.solve(MAT.toarray(), -np.matmul(dphidp,deltaP))