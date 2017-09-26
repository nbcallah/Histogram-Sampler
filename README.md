#Histogram Sampler

## Synopsis

Histogram Sampler is a library to randomly sample a histogram. A histogram is a list of frequency by bin, and Histogram Sampler will randomly generate a samples of an original histogram's distribution.

## Code Example

```
std::random_device rd;
std::mt19937_64 gen(1447);
std::uniform_int_distribution<uint64_t> randGen(0, std::numeric_limits<uint64_t>::max());

histGen* testGenerator;
uint64_t freq[10] = {1, 3, 3, 7, 9, 6, 4, 2, 3, 1};
const char *bins[10] = {"infrared", "red", "orange", "yellow", "sour", "green", "teal", "blue", "violet", "ultraviolet"};
testGenerator = createGen(freq, 10);
for(int i = 0; i < 10; i++) {
    uint32_t index = genIndex(testGenerator, randGen(gen));
    while(index == testGenerator->nEntries - 1) {
        index = genIndex(testGenerator, randGen(gen));
    }
    printf("Color #%d: %s\n", i, bins[index]);
}
...
freeGen(testGenerator);

```

## Motivation

When the underlying distribution of a dataset is unknown it is sometimes useful to sample a histogram. For example, the distribution of dog coat colors can be well-measured by compiling population statistics from shelters. However, the distribution cannot be described by any basic probability distributions. Sampling the histogram of dog coat colors can allow sample populations to be generated from a larger dataset.

The library uses the "Alias Method" decribed at this URL: http://www.keithschwarz.com/darts-dice-coins/ but modified to use modular arithmetic and unsigned integers instead of doubles. Assuming perfect random numbers where all bits are uniformly random (may not be a good assumption), this should ensure that all samples are perfectly fair at the expense of rejecting some portion of random numbers.

We want to generate samples of a histogram (probabilities as a function of bin). This means we want to randomly pick a segment of the histogram proportional to its area. One way to do this is to "stack" bin contents on top of one another so that the histogram is now a rectangle. because the histogram is now a rectangle, it can be addressed using 2 uniform variates. One indexes the bin number, and the other rolls a dice on whether to pick the original bin content or the stacked bin.

Instead of using floats to index and get probabilities, split a generated 64 bit integer into 2 32 bits and use one for index and one for probability roll. Add an extra bin to re-roll probability to ensure rectangular histogram.

## Installation

An example program (and makefile) is provided in the test directory. The library is a single header file and a single source file.

## API Reference

    struct histGen
Contains the shape parameters of the histogram for random generation, in addition to arrays of the frequency of bins.
    struct histGen* createGen(uint64_t* histogram, uint32_t n)
Returns a struct with a populated histogram. If there are problems with generation, returns NULL. The number of bins must be less than UINT32_MAX.
    uint32_t genIndex(struct histGen* gen, uint64_t u)
Returns an index from 0 to n-1. If the given uniform integer cannot generate a sample (it may be too large or it samples the dead space), then n is returned. In this case, another uniform integer must be supplied and tested again until an index < n is generated.
    void freeGen(struct histGen* gen)
Frees space used inside of histGen struct.

## Tests

Compile the source in the test directory and run Test. Output should be 20 colors of dog coats.

## Contributors

As this is a small project, I will not actively maintain it. If you find this code, use it, and find a bug, feel free to let me know.

## Authors

Nathan Callahan

## License

This project is licensed under the MIT License - see the LICENSE.md file for details
