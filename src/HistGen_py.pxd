from libc.stdint cimport uint32_t, uint64_t

cdef extern from "../inc/HistGen.h":
	cdef struct histGen:
		pass
	
	histGen* createGen(uint64_t* histogram, uint32_t n)
	uint32_t genIndex(histGen* gen, uint64_t u)
	void freeGen(histGen* gen)
