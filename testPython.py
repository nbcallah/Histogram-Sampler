#!/usr/bin/python

import HistGen_py
import numpy

myHist = [100, 300, 300, 700, 900, 600, 400, 200, 300, 100]
myBins = ["infrared", "red", "orange", "yellow", "sour", "green", "teal", "blue", "violet", "ultraviolet"]
myTest = HistGen_py.HistGen(myHist)

for i in range(0,10):
	index = myTest.genIndex(numpy.random.randint(0, high=numpy.iinfo(numpy.uint64).max, dtype='uint64'))
	while index == len(myHist):
		index = myTest.genIndex(numpy.random.randint(0, high=numpy.iinfo(numpy.uint64).max, dtype='uint64'))
	print(myBins[index])