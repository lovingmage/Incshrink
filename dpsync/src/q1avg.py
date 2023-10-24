#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# This file is used to calculate the averate statistics
"""
Created on Sun Aug 30 15:44:37 2020

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





T = 15
eps = 0.01
flk = 25
fls = 15
arr = fk.read_workload()
mode = 2

# Timer method
#lgp, sync_log, dummy_log, flush_dummy_log = fk.timer_q1(40, eps, arr, flk, fls, mode = mode)
lgp_a, sync_log_a, dummy_log_a, flush_dummy_log_a = sp.sparse(T, eps, arr, flk, fls, mode = mode)



lgp_ot, fil_lgp_ot = ss.oto_q1(arr, 250)



rep_mean_lgp = []
rep_max_lgp = []
rep_med_lgp = []
rep_dummy = []
rep_size = []
for i in range(10):
    lgp, sync_log, dummy_log, flush_dummy_log = sp.sparse(T, eps, arr, flk, fls, mode = mode, span = 1)
    rep_mean_lgp.append(np.mean(lgp))
    rep_max_lgp.append(max(lgp))
    rep_med_lgp.append(np.median(lgp))
    rep_dummy.append(sum(dummy_log) + sum(flush_dummy_log))
    rep_size.append(sum(sync_log))
    

print(np.mean(rep_mean_lgp))
print(np.mean(rep_max_lgp))
#print(np.mean(rep_size))


msz = 20
recz = 1
df_sz = []
for i in range(len(sync_log)):
    if i % msz == 0:
        df_sz.append(recz*sum(sync_log[0:i]))
        
df_sz.append(recz*sum(sync_log))
#get the mean size of timer method
print("timer mean size:", np.mean(df_sz[1:]))
print("timer max size:", df_sz[-1])





#get the mean size of db for baseline

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
        df_true.append(recz*sum(true[0:i]))
df_true.append(recz*sum(true))

print("sur mean size:", np.mean(df_true[1:]))
print("sur max size:", df_true[-1])

####################
#timer mean size: 9793.930555555555
#timer max size: 19659
#sur mean size: 9467.421527777778
#sur max size: 19038
####################




###### oto mean query error perf
#q1 - 1929.47
#q2 - 9464.14
#total size - 1
# average execution 1x

###### set method
#q1 - 1
#q2 - 1
# total size - 43199
# mean execution 21600x

