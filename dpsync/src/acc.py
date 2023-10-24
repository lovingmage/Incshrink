#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Aug 30 13:52:01 2020

@author: lovingmage
"""


import timer as fk
import sync_set as ss
import lapmec as lp
import numpy as np
import matplotlib.pyplot as plt
import _pickle as pickle
import sparse as sp

from operator import itemgetter



T = 20
eps = 0.5
flk = 50
fls = 15
arr = fk.read_workload()
mode = 1

# Timer method
lgp, sync_log, dummy_log, flush_dummy_log = fk.timer_q1(40, eps, arr, flk, fls, mode = mode)
lgp_a, sync_log_a, dummy_log_a, flush_dummy_log_a = sp.sparse(T, 0.01, arr, flk, fls, mode = mode)



lgp_ot, fil_lgp_ot = ss.oto_q1(arr, 250, mode = mode)

#print(max(abs(lp.gen_laplace_once(1, 173))))

print(np.mean(lgp), np.std(lgp), np.max(lgp))
print(np.mean(lgp_a), np.std(lgp_a), np.max(lgp_a))
# plot query error v.s. time
x = np.arange(1, 43199, 250)

plt.plot(x, lgp, 'o', label='DP-Timer,  $\epsilon_{u} = 0.5$, $\epsilon_{q} = 1$', c='red', markersize = 7, alpha = 0.5)
plt.plot(x, lgp_a, 'o', label='DP-ANT,  $\epsilon_{u} = 0.5$, $\epsilon_{q} = 1$', c='orange', markersize = 7, alpha = 0.5)
plt.plot(x, fil_lgp_ot[:-1], 'o', label='OTO, $\epsilon_{u} = 0$, $\epsilon_{q} = 1$', c='blue', markersize = 7, alpha = 0.3)
plt.plot(x, abs(lp.gen_laplace_once(200, 173)), 'o', label='SET, $\epsilon_{u} = 0$, $\epsilon_{q} = 1$', c='green', markersize = 7, alpha = 0.3)
plt.plot(x, abs(lp.gen_laplace_once(200, 173)), 'x', label='SUR, $\epsilon_{u} = \infty$, $\epsilon_{q} = 1$', c='black', markersize = 7, alpha = 0.4)


plt.yscale('log')
plt.legend(bbox_to_anchor=(0.45,0.40))
plt.xlabel('Time stamp', fontsize=12)
plt.ylabel('L1 error', fontsize=12)

'''
plt.text(4500, 1, 'SET and SUR has 0 logical gap.', style='italic',
        bbox={'facecolor': 'black', 'alpha': 0.2, 'pad': 8})
'''

#plt.annotate('', xy=(2500, 1), xytext=(2500, 4), arrowprops=dict(facecolor='black', shrink=0.05))

plt.grid(True)
#plt.title('Query accuracy v.s. Time')
#plt.savefig('scatter_plot_q2_acc_crypte.pdf')  
plt.show()






'''
# plot query error v.s. time
x = np.arange(1, 43199, 250)

plt.plot(x, lgp, '^', label='DP Timer,  $\epsilon$-DP', c='red', markersize = 5)
plt.plot(x, fil_lgp_ot[:-1], '.', label='OTO method, $\infty$-DP', c='blue', markersize = 5)
plt.plot(x, [1]*173, '.', label='SET method, $\infty$-DP', c='green', markersize = 5)
plt.plot(x, [1]*173, '--', label='SUR method, 0-DP', c='black', markersize = 5)


plt.yscale('log')
plt.legend(loc='best')
plt.xlabel('Time stamp', fontsize=12)
plt.ylabel('L1 error', fontsize=12)
#plt.title('Query accuracy v.s. Time')
plt.savefig('line_plot_q2.pdf')  
plt.show()
'''




