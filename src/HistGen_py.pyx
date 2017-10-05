cimport HistGen_py
cimport cython
from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free
from libc.stdint cimport uint32_t, int64_t

cdef class HistGen:
	cdef histGen* gen
	def __init__(self, hist):
		self.gen = NULL
		cdef uint32_t num = <uint32_t>len(hist)
		cdef uint64_t* c_hist = <uint64_t*>PyMem_Malloc(len(hist)*cython.sizeof(uint64_t))

		if c_hist is NULL:
			raise MemoryError()

		for i in xrange(len(hist)):
			c_hist[i] = hist[i]
			
		self.gen = HistGen_py.createGen(c_hist, num)
		
		if self.gen is NULL:
			raise ValueError()
		
		PyMem_Free(c_hist)
		
	def genIndex(self, uint64_t u):
		if self.gen is NULL:
			raise ValueError()
		return HistGen_py.genIndex(self.gen, u)
	
	def __del__(self):
		HistGen_py.freeGen(self.gen)
		self.gen = NULL