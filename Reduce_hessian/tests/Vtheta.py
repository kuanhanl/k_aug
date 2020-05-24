# -*- coding: utf-8 -*-
"""
Created on Tue May 12 16:26:16 2020

@author: greg6
"""

from scipy.sparse import coo_matrix 

nt = 3
nw = 2
nd = nt*nw

row = []
col = []
data = []



for i in range(nt):
    for j in range(nw):
        # val = sum(v_array[k] * s_array[j * nc + k] ** 2 for k in range(nc)) + v_device
        val = 8
        row.append(i * nw + j)
        col.append(i * nw + j)
        data.append(val)
        # Vd_dense[i*nw+j,i*nw+j] = val
        for p in range(nw):
            if j != p:
                # val = sum(v_array[k] * s_array[j * nc + k] * s_array[p * nc + k] for k in range(nc))
                val = 5
                row.append(i * nw + j)
                col.append(i * nw + p)
                data.append(val)
                # Vd_dense[i*nw+j,i*nw+p] = val
                
Vd_matrix = coo_matrix((data, (row, col)), shape=(nd, nd)).tocsr()