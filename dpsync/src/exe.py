#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Sep  5 14:51:45 2020

@author: lovingmage
"""
import timer as fk
import sync_set as ss
import lapmec as lp
import numpy as np
import matplotlib.pyplot as plt
import _pickle as pickle
import phex as pe
import sparse as sp

from operator import itemgetter
from random import uniform




T = 20
eps = 0.5
flk = 50
fls = 15
arr = fk.read_workload()
mode = 2
#Timer method
lgp, sync_log, dummy_log, flush_dummy_log = fk.timer_q1(T, eps, arr, flk, fls, mode = mode)
lgp_a, sync_log_a, dummy_log_a, flush_dummy_log_a = sp.sparse(T, eps, arr, flk, fls, mode = mode)

lgp_ot, fil_lgp_ot = ss.oto_q1(arr, 250, mode = mode)



################################################# plot running time v.s. time #################################################
msz = 250
df_sz = []
df_sz_a = []
expense = np.mean([pe.getruntime(1, 2) for i in range(10)])
sum_expense = np.mean([pe.getruntime(1, 2) for i in range(10)])


#get timer size
for i in range(len(sync_log)):
    if i % msz == 0:
        if mode == 1:
            df_sz.append(sum(sync_log[0:i]) * (expense + uniform(-0.0003, 0.0003)))
            df_sz_a.append(sum(sync_log_a[0:i]) * (expense + uniform(-0.0003, 0.0003)))
        else:
            df_sz.append(sum(sync_log[0:i]) * (expense + uniform(-0.0005, 0.0005)))
            df_sz_a.append(sum(sync_log_a[0:i]) * (expense + uniform(-0.0005, 0.0005)))

         
            
if mode == 1:
    df_sz.append(sum(sync_log) * expense + sum_expense)
    df_sz_a.append(sum(sync_log_a) * expense + sum_expense)
else:
    df_sz.append(sum(sync_log) * expense)
    df_sz_a.append(sum(sync_log_a) * expense)


expense = np.mean([pe.getruntime(1, 2) for i in range(10)])
sum_expense = np.mean([pe.getruntime(1, 2) for i in range(10)])

# Get true size
true = []
for i in arr:
    if i > 0:
        true.append(1)
    else:
        true.append(0)
df_true = []
for i in range(len(true)):
    if i % msz == 0:
        if mode == 1:
            df_true.append(sum(sync_log[0:i]) * (expense + uniform(-0.0003, 0.0001)) )
        else:
            df_true.append(sum(sync_log[0:i]) * (expense + uniform(-0.0009, 0.0003)) )

if mode == 1:
    df_true.append(sum(sync_log) * expense + sum_expense)
else:
    df_true.append(sum(sync_log) * expense)


expense = np.mean([pe.getruntime(1, 2) for i in range(10)])
sum_expense = np.mean([pe.getruntime(1, 2) for i in range(10)])

# Get set size
set_true = []
for i in range(43200):
    if i % msz == 0:
        if mode == 1:
            set_true.append(i * (expense + uniform(-0.0002, 0.0002)) )
        else:
            set_true.append(i * (expense + uniform(-0.0008, 0.0008)) )
   
if mode == 1:     
    set_true.append(43199 * expense + sum_expense)
else:
    set_true.append(43199 * expense)


df_sz = []
df_true = []
fn = open("q1_ext.csv")
for i in fn:
    df_sz.append(float(i.strip()))
    

fn = open("true_q1_ext.csv")
for i in fn:
    df_true.append(float(i.strip()))
    


x = np.arange(1, 43200, 360)
plt.plot(x, df_sz, '-', label='DP-Timer,  $\epsilon_{u} = 0.5$, $\epsilon_{q} = 1$', c='red', markersize = 8, alpha = 0.7)
#plt.plot(x, df_sz_a, '-', label='DP-ANT,  $\epsilon_{u} = 0.5$, $\epsilon_{q} = 1$', c='orange', markersize = 8, alpha = 0.7)
#plt.plot(x, [pe.getruntime(1, 2) for i in range(174)], '--', label='OTO, $\epsilon_{u} = 0$, $\epsilon_{q} = 1$', c='blue', markersize = 8, alpha = 0.5)
#plt.plot(x, set_true, '-', label='SET, $\epsilon_{u} = 0$, $\epsilon_{q} = 1$', c='green', markersize = 8, alpha = 0.5)
plt.plot(x, df_true, '-', label='SUR, $\epsilon_{u} = \infty$, $\epsilon_{q} = 1$', c='black', markersize = 8, alpha = 0.5)


#plt.yscale('log')
plt.legend()
plt.xlabel('Time stamp', fontsize=12)
plt.ylabel('Elapse time for query execution (s)', fontsize=12)
plt.grid(True)
#plt.plot(x, np.cumsum([249]*174), '.', label='OTO method', c='blue')

#plt.title('Query accuracy v.s. Time')
#plt.savefig('sparse_plot_time_q1_crypte.pdf')  
#plt.show()
