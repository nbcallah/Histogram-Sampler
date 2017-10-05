CC = gcc
CPP = g++
CPLUSPLUS11 = -std=c++11
CFLAGS = -O3 -arch i386 -arch x86_64

all: Test

clean:
	find . -type f -name '*.o' -delete
	rm -f Test
	
Test: Test.cpp histGen.o
	$(CPP) $(CFLAGS) $(CPLUSPLUS11) -o Test Test.cpp ./histGen.o
	
histGen.o: ../src/histGen.c ../inc/histGen.h
	$(CC) $(CFLAGS) -c -o ./histGen.o ../src/histGen.c
