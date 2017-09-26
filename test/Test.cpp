#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <random>
#include <limits>
#include <fstream>
#include <sstream>

extern "C" {
	#include "../inc/HistGen.h"
}

template<typename T, typename F>
void loadTextHistogram(histGen* &gen, std::string fName, std::vector<T> &vec, F f);

int main(int argc, char** argv) {
	histGen* testGenerator; //Will store generator
	std::vector<std::string> resultsVector; //Stores Brick colors
	
	auto func = [](std::string s)->std::string{return s;}; //Returns the bin as a string
	loadTextHistogram(testGenerator, "./test_hist.csv", resultsVector, func); //Load up test_hist.csv as a histogram (vectors of bins and frequencies) and create a histGen struct.
	
	//create random generator
	std::random_device rd;
    std::mt19937_64 gen;
	std::vector<uint64_t> seed;
	seed.push_back(2736687128);
	seed.push_back(234302120);
	seed.push_back(3355772407);
	seed.push_back(657836083);
	std::seed_seq sseq(seed.begin(), seed.end());
    gen.seed(sseq);
    std::uniform_int_distribution<uint64_t> randGen(0, std::numeric_limits<uint64_t>::max());
	
	//Sample 20 random Brick colors
	for(int i = 0; i < 20; i++) {
		//Get one index
		uint32_t index = genIndex(testGenerator, randGen(gen));
		//If the index is the last entry, that means the provided random number won't work.
		while(index == testGenerator->nEntries - 1) { //Keep providing random numbers until it works
			index = genIndex(testGenerator, randGen(gen));
		}
		printf("Brick #%d: %s\n", i, resultsVector[index].c_str());
	}
	freeGen(testGenerator);
	return 0;
}

//Template function to load a histogram in csv format.
//First column is the bin label, which can by anything.
//Second column is frequency, which is a uint64_t integer.
template<typename T, typename F>
void loadTextHistogram(histGen* &gen, std::string fName, std::vector<T> &vec, F f) {
	std::vector<uint64_t> hist;
	std::ifstream infile(fName);
	std::string line;
	//parse each line of the file
	while(std::getline(infile, line)) {
		std::istringstream iss(line);
		std::vector<std::string> strings;
		std::string s;
		//tokenize by comma
		while(std::getline(iss, s, ',')) {
			//make vector of first column and second column as strings
			strings.push_back(s);
		}
		//If we violate 2 columns, give up
		if(strings.size() != 2) {
			gen = NULL;
			vec.empty();
		}
		//Add the frequency to the histogram, and the bin label to the bin label vector.
		hist.push_back(std::stoull(strings[1]));
		vec.push_back(f(strings[0]));
	}
	assert(hist.size() > 0);
	//Create the histogram generation struct.
	gen = createGen(&(hist[0]), hist.size());
}