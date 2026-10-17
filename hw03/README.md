# HW3: Pointer library

This folder has my pointer library for ECE 361 Homework 3. It has four
functions that work with floats and arrays using pointers:

- swap_float: swaps two floats by using their addresses
- minmax: finds the smallest and largest value in an array
- find_above: returns a pointer to the first value bigger than a threshold
- reverse: reverses an array in place with two pointers

## Files

- ptr.h: the function prototypes, with a comment for each one
- ptr.c: the code for the four functions (pointer arithmetic, no a[i])
- tests/test_ptr.c: the tests
- Makefile: builds the code and runs the tests
- AI_USAGE.md: how I used AI tools for this homework

## How to build

Go into the hw03 folder and run:

    make

This makes ptr.o and tests/test_ptr using gcc with -std=c11 -Wall -Wextra.
It should print no warnings.

## How to test

    make test

This runs the tests. Each test prints PASS or FAIL, and the last line is a
summary. The program exits with a nonzero code if any test fails.

## How to clean

    make clean

This removes ptr.o and tests/test_ptr.
