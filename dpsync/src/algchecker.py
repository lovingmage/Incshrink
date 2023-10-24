# -*- coding: utf-8 -*-
"""
Created on Tue Feb 25 10:51:53 2020

@author: skiph
"""

import lapmec as lp
import numpy as np
import matplotlib.pyplot as plt


# Function to generate fully random workload
def rd_workload(arrSize):
    return np.random.randint(2, size = arrSize)

# Interface to generate random workload with specified arrival rate
# @ arrSize - Workload stream length
# @ pr - Record arrival rate
def gen_workload(arrSize, pr):
    arr = []
    for i in range(arrSize):
        arr.append( np.random.choice(np.arange(0, 2), p=[1-pr, pr]) )
    return arr
    
    
# Cascading Buffer Method
# @Param bufSize - the threshold \theta size
# @Param e, k - privacy parameters, epsilon, k respectively
# @Param workload - The update workload for running cb method.
def casecade_buffer_wdelay(bufSize, e, k, workload):
    # Init Caches, L1,L2 cache.
    c1buf = 0
    c2buf = 200
    count = 0
    
    uinfo = []
    log_c2buf = []
    log_c1buf = []
    
    # Split privacy budget e1-svt, e2-private releasing.
    e1 = (8.0/9) * e
    e2 = (2.0/9) * e
    
    T = bufSize + 2 * lp.gen_laplace_once(e1*0.5, k)
    
    records = 0
    
    for i in range( len(workload) ):
        z = lp.gen_laplace_once(e2*0.25, k)
        z = z[0]
        if ( c1buf + z < T):
            c1buf = c1buf + workload[i]
        else:
            z2 = lp.gen_laplace_once(e2*0.5, k)
            z2 = z2[0]
            if (z2 >= 0 ):
                c2buf = max(c2buf - z2, 0)
            else:
                c2buf = c2buf - z2
                
            uinfo.append(c1buf + z2)
            log_c2buf.append(abs(c2buf))
            log_c1buf.append(c1buf)
            count += 1
            
            T = bufSize + 2 * lp.gen_laplace_once(e1*0.5, k)
            records = records + c1buf + z2
            c1buf = 0
            #print (c2buf)
    
    #count : how many updates in total
    #uinfo : update size of each time stampe
    #log_c1buf: log of true counts at each updates
    #log_c2buf: flush cache size
    #records: total records sychronized
    return count, uinfo, log_c1buf, log_c2buf, records


    
#Timmer Method
# @Param t - the timer for two consecutive updates
# @Param e, k - privacy parameters, epsilon, k respectively
# @Param workload - The update workload for running cb method.
def timer_method(t, e, k, workload):
    c1buf = 0
    c2buf = 100
    count = 0
    
    uinfo = []
    log_c2buf = []
    log_c1buf = []
    records = 0
    
    for i in range(len(workload)):
        c1buf = c1buf + workload[i]
        if((i % t) == 1):
            count+=1
            z = lp.gen_laplace_once(e, k)
            z = z[0]
            
            if (z >= 0 ):
                c2buf = max(c2buf - z, 0)
            else:
                c2buf = c2buf - z
                
            log_c2buf.append(abs(c2buf))
            log_c1buf.append(c1buf)
            uinfo.append(z + c1buf)
            
            records = records + z + c1buf
            c1buf = 0
            #print(log_c2buf)
            
        
    #count : how many updates in total
    #uinfo : update size of each time stampe
    #log_c1buf: log of true counts at each updates
    #log_c2buf: flush cache size
    #records: total records sychronized
    return count, uinfo, log_c1buf, log_c2buf, records
    
                                
                    


'''
The Following interfaces simulates update pattern for running DP strategies.

'''
# Simulate uppdate pattern when launching DP-ANT
# @p - record arrival rate, default 0.1
# @eps - privacy budget, default 0.5
# @bufSize - workload length, default 200.

def sim_alg2(p = 0.1, eps = 1, bufSize = 200, loadSacle = 10000, rep = 10):
    ret = []
    usize_global = []
    c1log_global = []
    c2log_global = []
    records_global = []

    for i in range(rep):
        arr = gen_workload(loadSacle, p)
        cnt, uinfo, c1log, c2log, records = casecade_buffer_wdelay(bufSize, eps, 1, arr)
        c1log_global = c1log_global + c1log
        usize_global = usize_global + uinfo
        c2log_global = c2log_global + c2log
        
        #Here are outputs
        records_global.append(records);
        
        #print (c2log)
        
    upper = np.percentile(records_global, 95)
    lower = np.percentile(records_global, 5)
    
        
    return c1log_global, usize_global, c2log_global, upper, lower




def sim_alg1(p = 0.1, eps = 1, timer = 20, loadSacle = 10000, rep = 10):
    ret = []
    usize_global = []
    c1log_global = []
    c2log_global = []
    records_global = []
    for i in range(rep):
        arr = gen_workload(loadSacle, p)
        cnt, uinfo, c1log, c2log, records = timer_method(timer, eps, 1, arr)
        c1log_global = c1log_global + c1log
        usize_global = usize_global + uinfo
        c2log_global = c2log_global + c2log
        
        #Here are outputs
        records_global.append(records)
    
    upper = np.percentile(records_global, 95)
    lower = np.percentile(records_global, 5)
    
    print(np.mean(records_global))
        
    return c1log_global, usize_global, c2log_global, upper, lower
    

def sim_c2buf(n = 100, e = 0.5, rep = 30, c=200, T = 20):
    log_buf2 = []
    for i in range(rep):
        buf2 = 100
        for i in range(20):
            z = lp.gen_laplace_once(e, 1)
            z = z[0]
            buf2 += z
        log_buf2.append(buf2)
    upper = np.percentile(log_buf2, 95)
    lower = np.percentile(log_buf2, 5)
            
    return upper, lower

'''
log = []
for i in range(10):
    z = lp.gen_laplace_once(0.4, 500)
    c = np.sum(z)
    log.append(c)
    
print(5000 + np.mean(np.abs(log)))
print(np.std(np.abs(log)))
    

'''
#c1_data, usize_data, c2_data = sim_alg1(eps = 1, rep=1)
accu = 0
log = []
global_log = []
num = 10000 * 0.1
for j in range(10):
    for i in range(500):
        z = lp.gen_laplace_once(1, 1)
        z = z[0]
        if accu < -35:
            accu = 0
        accu = accu + z
        
        if accu < 0:
            log.append(2 + np.abs(accu))
            
        if accu >=0:
            log.append(2)
    global_log.append(np.mean(log))
        
print(np.mean(global_log))
        
'''
#x = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
labels = ['0.1', '0.3', '0.5', '0.7', '0.9', '1.1', '1.3', '1.5', '1.7', '1.9']
fig1, ax1 = plt.subplots()
ax1.set_title('L2 Cache Size')
ax1.boxplot(log_c1,showmeans=True)
ax1.set_xticklabels(labels, minor=False)
ax1.set_title('Size of L2 Cache (p=0.5, theta=50, T = 10000)')
ax1.set_ylabel('Size of L2 Cache')
ax1.set_xlabel('epsilon')



log_upper = [] 
log_lower = []
for i in range(10):
    t = 10 + i * 10
    #c1_data, usize_data, c2_data, upper, lower = sim_alg2(eps=ep)
    upper, lower = sim_c2buf(e = 0.5, T = t)
    log_upper.append(upper)
    log_lower.append(lower)

print(log_upper)
print(log_lower)
'''


     
