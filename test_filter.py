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

class TestFilter(unittest.TestCase):
    def test_filter_timer(self):
        db = './data/sample_db/sample.txt'
        data = dsp.load_data(db)
        dsp.timer(db, blk_sz=240, T=5)
        view = dsp.debug_ss_view()
        cache = dsp.debug_ss_cache()
        true_out = [elem for elem in data if elem > 5]
        
        ctrue = collections.Counter(np.array(true_out))
        cview = collections.Counter(np.array(view))
        ccache = collections.Counter(np.array(cache))
        
        print("\n")
        print("[*] Filter (groub by) query output:")
        print("Ground Truth : ", sorted(ctrue.items()))
        print("Query output : ", sorted(cview.items()))
        print("Cache snapshot : ", sorted(ccache.items()))
        print("\n")
        
        #print(collections.Counter(np.array(cache)))
        dsp.reset_objects()
        
    def test_filter_ant(self):
        db = './data/sample_db/sample.txt'
        data = dsp.load_data(db)
        dsp.ant(db, blk_sz=240)
        view = dsp.debug_ss_view()
        cache = dsp.debug_ss_cache()
        true_out = [elem for elem in data if elem > 5]
        
        ctrue = collections.Counter(np.array(true_out))
        cview = collections.Counter(np.array(view))
        ccache = collections.Counter(np.array(cache))
        
        print("\n")
        print("[*] Filter (groub by) query output:")
        print("Ground Truth : ", sorted(ctrue.items()))
        print("Query output : ", sorted(cview.items()))
        print("Cache snapshot : ", sorted(ccache.items()))
        print("\n")
        
        #print(collections.Counter(np.array(cache)))
        dsp.reset_objects()
    
    '''
    def test_filter_large(self):
        db = './data/sample_db/sample_large.txt'
        data = dsp.load_data(db)
        dsp.timer(db, blk_sz=2400, T=10)
        view = dsp.debug_ss_view()
        cache = dsp.debug_ss_cache()
        true_out = [elem for elem in data if elem > 5]
        
        ctrue = collections.Counter(np.array(true_out))
        cview = collections.Counter(np.array(view))
        ccache = collections.Counter(np.array(cache))
        
        print("\n")
        print("[*] Filter (groub by) query output:")
        print("Ground Truth : ", sorted(ctrue.items()))
        print("Query output : ", sorted(cview.items()))
        print("Cache snapshot : ", sorted(ccache.items()))
        
        #print(collections.Counter(np.array(cache)))
        dsp.reset_objects()
    '''
        
        
if __name__ == '__main__':
    unittest.main()
