#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Dec 18 12:56:15 2021

@author: lovingmage
"""
import random

left = set([random.randint(1,10000) for elem in range(10000)])
right = set([random.randint(1,500) for elem in range(500)])

f = open('./sample_db/join/c.txt', 'w')
for elem in list(right):
    f.write(str(elem)+'\n')
    
f = open('./sample_db/join/d.txt', 'w')
for elem in list(right):
    f.write('0\n')