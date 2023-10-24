#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Aug 29 22:12:56 2020

@author: lovingmage
"""



import lapmec as lp
import numpy as np
import matplotlib.pyplot as plt
import _pickle as pickle

def read_workload():
    arr = []
    fn = open("workload_taxi_pic.txt", "r")
    for i in fn:
        arr.append(int(i.strip()))
    return arr

# k - flush after k times
# s - flush size
def set_q1(workload):
    #data structures to store elements
    lcache = []
    lgp = []
    sync_log = []
    dummy_log = []
    
    for i in range(len(workload)):
        #push to the local cache
        if workload[i] == 0:
            dummy_log.append(1)
        else:
            dummy_log.append(0)
            
        lgp.append(0)
        sync_log.append(1)
        
    #print(np.sum(dummy_log))
        
    return lgp, sync_log, dummy_log


# k - flush after k times
# s - flush size
def oto_q1(workload, ki, mode = 1):
    #data structures to store elements
    lgp = []
    fil_lgp = [1]

    for i in range(len(workload)):
        #push to the local cache
        #if workload[i] > 0:
        if mode == 1:
            if workload[i] > 50 and workload[i] < 100:
                lgp.append(1)
            else:
                lgp.append(0)                
        else:
            if workload[i] > 0:
                lgp.append(1)
            else:
                lgp.append(0)
            
        if i % ki == 0:
            z2 = lp.gen_laplace_once(200, 1)
            #for simulate oblidb
            #fil_lgp .append(sum(lgp))
            #for simulate crypte
            if mode == 3:
                fil_lgp.append(abs(int(0.39859154929577466*sum(lgp)) + z2[0]))
            else:
                fil_lgp.append(abs(sum(lgp) + z2[0]))

        
    return lgp, fil_lgp

'''
arr = read_workload()
#lgp, sync_log, dummy_log = set_q1(arr)
lgp,fil_lgp = oto_q1(arr, 2000)


fn_prefix = './out/q1/set-q1'
fn_str = '.out'

with open(fn_prefix+'-lgp'+fn_str, 'w') as fn1:
    for item in lgp:
        fn1.write("%s\n" % item)
        
with open(fn_prefix+'-sync_log'+fn_str, 'w') as fn2:
    for item in sync_log:
        fn2.write("%s\n" % item)
        
with open(fn_prefix+'-dummy_log'+fn_str, 'w') as fn3:
    for item in dummy_log:
        fn3.write("%s\n" % item)

        
x = np.arange(1, 23, 1)
plt.plot(x, fil_lgp, 'o--', label='Points removed', c='red')
plt.legend()
#plt.title('Masked and NaN data')
#plt.savefig('line_plot.pdf')  
plt.show()

'''
    