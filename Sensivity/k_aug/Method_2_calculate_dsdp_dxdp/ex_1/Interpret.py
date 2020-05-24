import numpy as np

dxdp = np.loadtxt('dxdp_.dat', usecols=range(2))
dsdp = np.loadtxt("dsdp_in_.in", usecols=range(1))

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

# dsdp = np.delete(dsdp, 1, 0)


deltaP = np.array([[0.5],
                  [0]])


x_star = np.array([[0.6326530612244898],
                  [0.38775510204081637],
                  [0.020408163265306097]])
deltax = dxdp.dot(deltaP)
x_est = x_star + deltax


deltas = dsdp.dot(deltaP)
print(deltas)

sp5 = np.array([[0.6327],
                [0.3878],
                [0.0204],
                [5],
                [1],
                [-0.1633],
                [-0.2857],
                [-0.1633],
                [0.1808]])

sp4 = np.array([[0.5765],
                [0.3775],
                [-0.0459],
                [4.5],
                [1],
                [-0.1327],
                [-0.3571],
                [-0.1327],
                [0.2099]])

deltas_check = np.subtract(sp4,sp5)
print("\n",deltas_check)

# print(x_est)

