# Histogram Sampler

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

When the underlying distribution of a dataset is unknown it is sometimes useful to sample a histogram. For example, the distribution of dog coat colors can be well measured by compiling population statistics from shelters. However, the distribution cannot be described by any basic probability distributions. Sampling the histogram of dog coat colors can allow sample populations to be generated from a larger dataset.

## Algorithm

The library uses the "Alias Method" decribed here: http://www.keithschwarz.com/darts-dice-coins/ but modified to use modular arithmetic and unsigned integers instead of doubles. Assuming perfect random numbers where all bits are uniformly random (n.b. this assumption may not be true for all psuedo random number generators), this should ensure that all samples are perfectly fair at the expense of rejecting some portion of input random numbers.

Sampling a histogram can also be seen as picking a bin from the histogram proportional to its area. One way to do this is to truncate the histogram height and "stack" bin contents on top of one another so that the histogram is now a rectangle. Because the histogram is now a rectangle, it can be addressed using 2 uniform variates. One indexes the bin number, and the other rolls a dice on whether to pick the original bin content or the stacked bin.

Instead of using floats to index and get probabilities, a 64 bit integer is split into 2 32 bit integers. To ensure fairness, a cutoff is used for the bin number and dice roll. The cutoff forces only an even number of modulus rollovers to occur and that there's no extra coverage for some bins or dice probabilities. Additionally, an extra bin is added to ensure the ability to make rectangular histograms. The cutoff probability is chosen such that there is extra space in the histogram: dead space for the extra bin. If the dead space is sampled, then the input random numbers are rejected.

## Installation

An example program (and makefile) is provided in the test directory. The library is a single header file and a single source file; compilation for use in other programs is left as an excercise to the reader.

## API Reference

### `struct histGen`
Contains the shape parameters of the histogram for random generation, in addition to arrays of the frequency of bins.

### `struct histGen* createGen(uint64_t* histogram, uint32_t n)`
Returns a struct with a populated histogram. If there are problems with generation, returns NULL. The number of bins must be less than UINT32_MAX.

### `uint32_t genIndex(struct histGen* gen, uint64_t u)`
Returns an index from 0 to n-1. If the given uniform integer cannot generate a sample (it may be too large or it samples the dead space), then n is returned. In this case, another uniform integer must be supplied and tested again until an index < n is generated.

### `void freeGen(struct histGen* gen)`
Frees space used inside of histGen struct.

## Tests

Compile the source in the test directory and run Test. Output should be 20 colors of dog coats.

## Contributors

As this is a small project, I will not actively maintain it. If you find this code, use it, and find a bug, feel free to let me know.

## Sources

For the test dataset, I have used play brick color data available at https://www.kaggle.com/rtatman/lego-database which was under the CC0 public domain license.

I have based the algorithms on a writeup from Keith Schwarz at http://www.keithschwarz.com/darts-dice-coins/

## Authors

Nathan Callahan

## License

This project is licensed under the MIT License - see the LICENSE.md file for details
