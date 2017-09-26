#include "../inc/HistGen.h"
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

struct histGen* createGen(uint64_t* histogram, uint32_t n) {
	if(n == UINT32_MAX) { //We need an extra bin to ensure perfectly rectangular histogram
		return NULL;
	}
	
	struct histGen* gen = (struct histGen *)malloc(sizeof(struct histGen));

	uint64_t sum = 0; //Sum of probabilities
	for(uint32_t i = 0; i < n; i++) {
		sum += histogram[i];
	}
	
	//Based on aliasing method described in http://www.keithschwarz.com/darts-dice-coins/
	
	//We want to generate samples of a histogram (probabilities as a function
	//of bin). This means we want to randomly pick a segment of the histogram
	//proportional to its area. One way to do this is to "stack" bin contents
	//on top of one another so that the histogram is now a rectangle. because
	//the histogram is now a rectangle, it can be addressed using 2 uniform
	//variates. One indexes the bin number, and the other rolls a dice on
	//whether to pick the original bin content or the stacked bin. The trick
	//is to generate the rectangular histogram.
	
	//Instead of using floats to index and get probabilities, split a
	//generated 64 bit integer into 2 32 bits and use one for index and one
	//for probability roll. Add an extra bin to re-roll probability to ensure
	//rectangular histogram.

	gen->nEntries = n+1;
	uint64_t cutoff = (sum / (n+1)) + 1; //To make things square, need to cut off high bins
	if(cutoff > UINT32_MAX) { //If the cutoff would be too high, can't generate random dice rolls.
		return NULL;
	}
	gen->cutoff = cutoff;
	
	uint64_t extras = cutoff * (n+1) - sum; //This is the "excess" probability (area) left over due to extra bin
	
	//We'll use modulus, so need to reject last portion of range to ensure uniformity
	gen->maxIndex = (n+1) * (UINT32_MAX / (n+1));
	gen->maxProb = cutoff * (UINT32_MAX / cutoff);
	
	//alias stores the identity of the stacked bins
	gen->alias = (uint32_t *)malloc(sizeof(uint32_t) * (n+1));
	//prob stores the probability of picking the initial bin over the stacked bin
	gen->prob = (uint32_t *)malloc(sizeof(uint32_t) * (n+1));
	
	//Create 2 arrays: One of bins which could fill up to the cutoff, one of bins less than cutoff
	uint64_t* smallProb = (uint64_t *)malloc(sizeof(uint64_t) * (n+1));
	uint32_t* smallIndex = (uint32_t *)malloc(sizeof(uint32_t) * (n+1));
	uint64_t* largeProb = (uint64_t *)malloc(sizeof(uint64_t) * (n+1));
	uint32_t* largeIndex = (uint32_t *)malloc(sizeof(uint32_t) * (n+1));
	uint32_t nSmall = 0;
	uint32_t nLarge = 0;
	
	//Sort into arrays
	for(uint32_t i = 0; i < n; i++) {
		if(histogram[i] < cutoff) {
			smallProb[nSmall] = histogram[i];
			smallIndex[nSmall] = i;
			nSmall++;
		}
		else {
			largeProb[nLarge] = histogram[i];
			largeIndex[nLarge] = i;
			nLarge++;
		}
	}

	//Sort excess area of histogram into small or large.
	if(extras < cutoff) {
		smallProb[nSmall] = extras;
		smallIndex[nSmall] = n;
		nSmall++;
	}
	else {
		largeProb[nLarge] = extras;
		largeIndex[nLarge] = n;
		nLarge++;
	}
	
	//While we have bins capable of stacking on top
	while(nSmall > 0) {
		//Take enough probability from tail of large array and stack onto current small.
		//This puts their sum up to cutoff, and therefore rectangular
		gen->alias[smallIndex[nSmall-1]] = largeIndex[nLarge-1];
		gen->prob[smallIndex[nSmall-1]] = smallProb[nSmall-1];
		
		largeProb[nLarge-1] -= (cutoff - smallProb[nSmall-1]);
		
		nSmall--;
		
		//If we made the large bin small by redistributing its probability, switch it to small.
		if(largeProb[nLarge-1] < cutoff) {
			smallIndex[nSmall] = largeIndex[nLarge-1];
			smallProb[nSmall] = largeProb[nLarge-1];
			nSmall++;
			nLarge--;
		}
	}

	//We should be left over with bins at the cutoff, so we'll set them all to be of cutoff height.
	while(nLarge > 0) {
		if(largeProb[nLarge-1] != cutoff) { //If we violate that assumption, return NULL
			freeGen(gen);
			free(smallProb);
			free(smallIndex);
			free(largeProb);
			free(largeIndex);
			return NULL;
		}
		gen->alias[largeIndex[nLarge-1]] = largeIndex[nLarge-1];
		gen->prob[largeIndex[nLarge-1]] = cutoff;
		nLarge -= 1;
	}
		
	free(smallProb);
	free(smallIndex);
	free(largeProb);
	free(largeIndex);
	
	return gen;
}

uint32_t genIndex(struct histGen* gen, uint64_t u) {
	//We take one uint64_t and extract 2x uint32_t
	uint32_t a;
	uint32_t b;
	a = u >> 32;
	b = (u << 32) >> 32;
	
	if(a > gen->maxIndex || b > gen->maxProb) {
		//If we are outside of where modulus will ensure uniformity, return the "extra" bin as rejection
		return gen->nEntries-1;
	}
	
	uint32_t index = a % (gen->nEntries);
	uint32_t prob = b % (gen->cutoff) + 1;
	
	return prob <= gen->prob[index] ? index : gen->alias[index];
}

void freeGen(struct histGen* gen) {
	free(gen->alias);
	free(gen->prob);
	free(gen);
}