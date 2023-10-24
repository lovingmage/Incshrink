#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep  3 21:03:32 2020

@author: lovingmage
"""


import lapmec as lp
import numpy as np
import matplotlib.pyplot as plt
import _pickle as pickle




def rd_workload(arrSize):
    return np.random.randint(2, size = arrSize)

def gen_workload(arrSize, pr):
    arr = []
    for i in range(arrSize):
        arr.append( np.random.choice(np.arange(0, 2), p=[1-pr, pr]) )
    return arr

def read_workload():
    arr = []
    fn = open("workload_taxi_pic.txt", "r")
    for i in fn:
        arr.append(int(i.strip()))
    return arr


# k - flush after k times
# s - flush size
def sparse(Th, e, workload, k, s, span = 250, mode = 1):
    #data structures to store elements
    lcache = []
    lgp = []
    sync_log = []
    dummy_log = []
    flush_dummy_log = []
    cnt = 0
    ccnt = 0
    
    #setup noisy threshold
    e1 = 0.5 * e
    e2 = e - e1 
    
    z = lp.gen_laplace_once(e1 * 0.5, 1)
    NTh = Th  + abs(z[0])
    #print(NTh)
    
    #start sync
    for i in range(len(workload)):
        #push to the local cache
        if workload[i] > 0:
            cnt = cnt + 1
            ccnt = ccnt + 1
            lcache.append(workload[i])
            
        #sync
        z = lp.gen_laplace_once(e1 * 0.25, 1)
        if cnt + z[0] >= NTh:
            #print(cnt + z[0])
            z = lp.gen_laplace_once(e2, 1)
            z = z[0]
            if cnt + z > 0:
                sync_log.append(cnt + z)
                if cnt + z >= len(lcache):
                    dummy_log.append(cnt + z - len(lcache))
                    lcache = []
                else:
                    lcache = lcache[(cnt + z):]
                    #print(len(lcache))
                    dummy_log.append(0)
            else:
                sync_log.append(0)
                
            cnt = 0
        else:
            sync_log.append(0)
            dummy_log.append(0)

        
        #flush    
        if i % (k*20) == 0:
            sync_log[i] = sync_log[i] + s
            if s >= len(lcache):
                flush_dummy_log.append(s - len(lcache))
                lcache = []
            else:
                lcache = lcache[s:]
                flush_dummy_log.append(0)
        else:
            flush_dummy_log.append(0)

                
         #query every 15/250 minutes
        # range query search pickup location in [50, 100]
        if i % span == 0:
            err = 0
            for j in lcache:
                if j > 50 and j < 100:
                    err = err + 1
            #lgp.append(err)
            #lgp.append(len(lcache))
            
            
            #for simulate crypte
            z2 = lp.gen_laplace_once(200, 1)
            if mode == 1:
                lgp.append(abs(err + sum(z2)))
            #if mode == 3:
            #    lgp.append(abs( int(0.44859154929577466*len(lcache)) + sum(z2)))
            else:
                lgp.append(abs(len(lcache) + sum(z2)))
            
        
    #print(len(lcache))
    return lgp, sync_log, dummy_log, flush_dummy_log

'''
T = 40
eps = 0.5
flk = 50
fls = 15
mode = 1
arr = read_workload()
lgp, sync_log, dummy_log, flush_dummy_log = sparse(T, eps, arr, flk, fls, mode = mode)

x = np.arange(1, 174, 1)
plt.plot(x, lgp, '-', label='Points removed', c='red')
plt.legend()
#plt.title('Masked and NaN data')
#plt.savefig('line_plot.pdf')  
plt.show()
'''