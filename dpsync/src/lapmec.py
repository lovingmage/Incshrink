import numpy as np
import random
import math
import time

# The following are the interfaces for file I/Os
# read data to a list
def readData(inputfile):
    D = []
    f = open(inputfile)
    for i in f.readlines():
        #print(i)
        D.append(i.split(','))
    return D


# Read data to a histogram   
def read_data_to_hist(file, mode = "age"):
    f= open(file)
    if mode == "age":
        HIT = np.zeros(100)
        for elem in f.readlines():
            index = elem.split(',')[1]
            HIT[int(index)] += 1
    if mode == "na":
        HIT = np.zeros(45)
        for elem in f.readlines():
            index = elem.split(',')[3]
            HIT[int(index)] += 1
    if mode == "gen":
        HIT = np.zeros(2)
        for elem in f.readlines():
            index = elem.split(',')[0]
            HIT[int(index)-1] += 1
    return HIT


def sum_ones(num):
    bin_string = bin(num)
    cnt_ones = 0
    for i in bin_string:
        if i == '1':
            cnt_ones += 1
        else:
            continue
    return cnt_ones


def convert_to_log_err(err):
    log10err = np.zeros(len(err))
    for i in range(len(err)):
        log10err[i] = math.log10(err[i] + 1)
    return log10err

#Generate Laplace Once
def gen_laplace_once(e, k):
    u=np.random.geometric(1-pow(2.713,-e), k)
    v=np.random.geometric(1-pow(2.713,-e), k)
    z=u-v
    return z

#Direct Simulate Generate noise 2 times
def gen_laplace(e, k):
    u=np.random.geometric(1-pow(2.713,-e), k)
    v=np.random.geometric(1-pow(2.713,-e), k)
    z=u-v
    
    u2=np.random.geometric(1-pow(2.713,-e), k)
    v2=np.random.geometric(1-pow(2.713,-e), k)
    z2=u2-v2
    
    return z + z2


def gen_index_noise(e, l, k):
    u=np.random.geometric(1-pow(2.713,-(e/l)), k)
    v=np.random.geometric(1-pow(2.713,-(e/l)), k)
    noisyAns=u-v
    #noisyAns2 = np.random.laplace(0, k/e, 5)
    return noisyAns
    
# This is the function that generates noise list as AND csp
def addLplace(trueList, e):
    u=np.random.geometric(1-pow(2.713,-e), size=len(trueList))
    v=np.random.geometric(1-pow(2.713,-e), size=len(trueList))
    z=u-v
    
    u2=np.random.geometric(1-pow(2.713,-e), size=len(trueList))
    v2=np.random.geometric(1-pow(2.713,-e), size=len(trueList))
    z2=u2-v2
    
    noisyList = trueList + z + z2
    
    return noisyList

# This is the new f1 score measure
def p2_err_measure(ref, predicted, histogram_t, delta, e):
    sum_cnt = 0
    slack = (1/e) * math.log(1/delta)
    #print (slack)
    #print (histogram_t)
    for j in predicted:
        if (j not in ref) and (histogram_t[j] < (histogram_t[ref[-1]] - slack )):
            sum_cnt += 1
            
    return (len(ref) - sum_cnt)/len(ref)

# Get top K 
def getTopK(data_hist, e=0.1, mode="t"):
    #True top-k mode
    if mode == "t":
        return data_hist.argsort()[::-1][:5]
    
    #noise range tree mode
    if mode == "nr":
        nhist = np.zeros(len(data_hist))
        for i in range(len(data_hist)):
            tmp_noise = 0
            for j in range(sum_ones(i+1)):
                tmp_noise += gen_laplace(e, 1)
            nhist[i] = data_hist[i] + tmp_noise
        return nhist.argsort()[::-1][:5]
    
    #noisy node
    if mode == "n":
        nhist = np.zeros(len(data_hist))
        for i in range(len(data_hist)):
            nhist[i] = data_hist[i] + gen_laplace(e, 1)
        return nhist.argsort()[::-1][:5]
    
    #noisy cdp mode
    if mode == "cdp":
        nhist = np.zeros(len(data_hist))
        for i in range(len(data_hist)):
            nhist[i] = data_hist[i] + gen_laplace(e, 1)/2
        return nhist.argsort()[::-1][:5]

