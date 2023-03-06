#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on March 16 12:02:48 2021

@author: lovingmage
"""

import os
import numpy as np
import time
import collections


'''
    Quick debug
'''
def debug_ss_insert():
    ss1 = load_data('./data/0.txt')
    ss2 = load_data('./data/1.txt')
    return (np.add(ss1, ss2))
    
def debug_ss_cache():
    ss1 = load_data('./data/cache_0')
    ss2 = load_data('./data/cache_1')
    return (np.add(ss1, ss2))
    
def debug_ss_view():
    ss1 = load_data('./data/view_0')
    ss2 = load_data('./data/view_1')
    return (np.add(ss1, ss2))
    
def debug_ss_cacnt():
    ss1 = load_data('./data/cacnt_0')
    ss2 = load_data('./data/cacnt_1')
    return (np.add(ss1, ss2))

def get_unsync_data():
    ss1 = load_data('./data/cache_0')
    ss2 = load_data('./data/cache_1')
    return (np.count_nonzero(np.add(ss1, ss2)))

def get_view_sz():
    ss1 = load_data('./data/view_0')
    ss2 = load_data('./data/view_1')
    return (len(np.add(ss1, ss2)))
    
def reset_objects():
    c0_fn = os.path.abspath('./data/cache_0')
    open(c0_fn, 'w').close()
    
    c1_fn = os.path.abspath('./data/cache_1')
    open(c1_fn, 'w').close()
    
    v0_fn = os.path.abspath('./data/view_0')
    open(v0_fn, 'w').close()
    
    v1_fn = os.path.abspath('./data/view_1')
    open(v1_fn, 'w').close()
    
    ca0_fn = os.path.abspath('./data/cacnt_0')
    with open(ca0_fn, "w") as data_file:
        data_file.write(str(10)+'\n')
        
    ca1_fn = os.path.abspath('./data/cacnt_1')
    with open(ca1_fn, "w") as data_file:
        data_file.write(str(-10)+'\n')
        
    th0_fn = os.path.abspath('./data/th_0')
    with open(th0_fn, "w") as data_file:
        data_file.write(str(110)+'\n')
        
    th1_fn = os.path.abspath('./data/th_1')
    with open(th1_fn, "w") as data_file:
        data_file.write(str(-10)+'\n')
        
    
    


'''
    Data operations, I/O
'''
def load_data(fn):
    data = []
    abs_fn = os.path.abspath(fn)
    f = open(abs_fn)
    for elem in f:
        data.append(int(elem))
    return data
    
# dummy secret share generation, 
# should replace 5 with random numbers
def generate_ss(data):
    ss_1 = []
    ss_2 = []
    for elem in data:
        ss_2.append(10)
        ss_1.append(elem-10)
    return ss_1, ss_2
    
def update_data(fn, data):
    abs_fn = os.path.abspath(fn)
    with open(abs_fn, "w") as data_file:
        for elem in data:
            data_file.write(str(elem)+'\n')
    
def insert(data):
    ss_0, ss_1 = generate_ss(data)
    fn_ds_0 = './data/0.txt'
    fn_ds_1 = './data/1.txt'
    update_data(fn_ds_0, ss_0)
    update_data(fn_ds_1, ss_1)
    
    
def partition_data(data, blk_sz=10):
    pdata = [data[i*blk_sz:(i+1)*blk_sz] for i in range(int(len(data)/blk_sz))]
    return pdata



'''
    Cache operations
'''
def cache_flush(sz):
    c0_fn = os.path.abspath('./data/cache_0')
    c1_fn = os.path.abspath('./data/cache_1')
    
    css_0 = load_data(c0_fn)
    css_1 = load_data(c1_fn)
    
    css_0 = css_0[0:sz]
    css_1 = css_1[0:sz]
    
    update_data(c0_fn, css_0)
    update_data(c1_fn, css_1)
    
def cache_resize(shift=1500):
    c0_fn = os.path.abspath('./data/cache_0')
    c1_fn = os.path.abspath('./data/cache_1')
    
    css_0 = load_data(c0_fn)
    css_1 = load_data(c1_fn)
    
    css_0 = css_0[0:0-shift]
    css_1 = css_1[0:0-shift]
    
    update_data(c0_fn, css_0)
    update_data(c1_fn, css_1)
    
def get_valid_tuples(in_list):
    dtuple = [elem for elem in in_list if elem > 0]
    return dtuple
    
        
    
    
    
'''
    Main operations
'''
def timer(db, T=3, blk_sz=10, eps=1):
    data = load_data(db)
    pdata = partition_data(data, blk_sz=blk_sz)
    query_error = []
    deferred_tuple = []
    
    for i in range(len(pdata)):
        print("[+] Current Timestamp => " + str(i))
        insert(pdata[i])
        cmd_trans = './bin/test_hermes 1 1234 & ./bin/test_hermes 2 1234'
        cache = list(debug_ss_cache())
        query_error.append(len([elem for elem in get_valid_tuples(cache) if elem >5]))
        
        os.system(cmd_trans)
        if ((i+1) % T) == 0:
            cmd_shrink = './bin/test_thor 1 1234 & ./bin/test_thor 2 1234'
            os.system(cmd_shrink)  
            cache = list(debug_ss_cache())
            deferred_tuple.append(len(get_valid_tuples(cache)))

            
            
    print (query_error)
    print (deferred_tuple)
        #cache_flush(20)
            
            
            
def ant(db, Th=500, blk_sz=10, eps=1):
    data = load_data(db)
    pdata = partition_data(data, blk_sz=blk_sz)
    
    for i in range(len(pdata)):
        print("[+] Current Timestamp => " + str(i))
        insert(pdata[i])
        cmd_trans = './bin/test_hermes 1 1234 & ./bin/test_hermes 2 1234'
        os.system(cmd_trans)
       
        cmd_shrink = './bin/test_sylvanas 1 1234 & ./bin/test_sylvanas 2 1234'
        os.system(cmd_shrink)  
        
        if ((i+1) % 10) == 0:
            cache_flush(20)
        
            
    
def debug_merge_join_1(left, right):
    left.sort()
    right.sort()
    i = 0
    j = 0
    res = []
    for i in range(len(left)):
        if (left[i] == right[j]):
            res.append(left[i]) 
            continue
            
        if (left[i] < right[j]):
            continue
        
        for k in range(j, len(right)-1, 1):
            j = j + 1
            if (left[i] <= right[j]):
                if (left[i] == right[j]):
                    res.append(left[i]) 
                break
    return res
                
            
def debug_merge_join_n(left, right, bound):
    left.sort()
    right.sort()
    i = 0
    j = 0
    res = []
    for i in range(len(left)):
        if (left[i] == right[j]):
            for k in range(bound):
                if (left[i] == right[j+k]):
                    res.append(left[i]) 
            continue
            
        if (left[i] < right[j]):
            continue
        
        for k in range(j, len(right)-1, 1):
            j = j + 1
            if (left[i] <= right[j]):
                if (left[i] == right[j]):
                    for k in range(bound):
                        if (j+k < len(right) and left[i] == right[j+k]):
                            res.append(left[i]) 
                break
    return res
    
db = './data/sample_db/sample.txt'
left = [8,5,6,4,3,1, 19, 33, 45, 19, 37, 31, 46, 99, 101, 73]
right = [6,7,8, 19, 19, 11, 73, 73, 73, 73, 73, 99, 101, 101]
#res = debug_merge_join_n(left, right, 8)
#print(res)
data = load_data(db)
#timer(db, blk_sz=10, T=2)
reset_objects()


#insert(data)