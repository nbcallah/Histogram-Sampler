#!/usr/bin/python

import HistGen_py

import numpy

myHist = [100,300,500,1000,500,300,100]

myTest = HistGen_py.HistGen(myHist)

testHist = [0,0,0,0,0,0,0]

for i in range(0,1000000):
	index = myTest.genIndex(numpy.random.randint(0, high=numpy.iinfo(numpy.uint64).max, dtype='uint64'))
	while index == len(myHist):
		index = myTest.genIndex(numpy.random.randint(0, high=numpy.iinfo(numpy.uint64).max, dtype='uint64'))
	testHist[index] += 1

myHistSum = 0
for content in myHist:
	myHistSum += content
	
testHistSum = 0
for content in testHist:
	testHistSum += content
	
for (my,test) in zip(myHist, testHist):
	print(1.0*my/(1.0*myHistSum), 1.0*test/(1.0*testHistSum))
