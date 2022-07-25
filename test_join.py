#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May 30 19:26:47 2021

@author: lovingmage
"""

import unittest
from unittest import TestSuite
import numpy as np
import random
import resource
import time
from dataclasses import dataclass
import dispatcher as dsp
import collections
import os

class TestFilter(unittest.TestCase):
    '''
    def test_sort_merge_join_1(self):
        left = set([random.randint(1,200) for elem in range(200)])
        right = set([random.randint(1,200) for elem in range(100)])
        res = dsp.debug_merge_join_1(list(left), list(right))
        self.assertEqual(left.intersection(right) - set(res), set([]))
        
    def test_sort_merge_join_2(self):
        left = set([random.randint(1,200000) for elem in range(200000)])
        right = set([random.randint(1,200000) for elem in range(200000)])
        res = dsp.debug_merge_join_1(list(left), list(right))
        self.assertEqual(left.intersection(right) - set(res), set([]))
    '''
        
    def test_timer(self):
        c_sz = []
        v_sz = []
        for i in range(2):
            c_sz.append(dsp.get_unsync_data())
            v_sz.append(dsp.get_view_sz())
            cmd_trans = './bin/test_anduin 1 1234 & ./bin/test_anduin 2 1234'
            os.system(cmd_trans)
            dsp.cache_resize()
            if ((i+1) % 5) == 0:
                cmd_shrink = './bin/test_thor 1 1234 & ./bin/test_thor 2 1234'
                os.system(cmd_shrink)  
                dsp.cache_flush(20)
            
                
        view = dsp.debug_ss_view()
        cache = dsp.debug_ss_cache()
        cview = collections.Counter(np.array(view))
        ccache = collections.Counter(np.array(cache))
        
        print("\n")
        print("[*] Join-filter query output at termination:")
        print("Query output : ", sorted(cview.items()))
        print("Cache snapshot : ", sorted(ccache.items()))
        print("\n")
        print("Output statistics\n")
        print(("|Acc|" + "\t" + "|Mean L1 Err|" + "\t" + "|Std L1 Err|").expandtabs(20))
        print(("|Values|" + "\t" + str(np.mean(c_sz)) + "\t" + str(np.std(c_sz)) ).expandtabs(20))
        print(("|Perf|" + "\t" + "|Mean View sz|" + "\t" + "|Max View sz|").expandtabs(20))
        print(("|Values|" + "\t" + str(np.mean(v_sz)) + "\t" + str(v_sz[-1])).expandtabs(20))
        #dsp.reset_objects()
        
        
if __name__ == '__main__':
    unittest.main()
