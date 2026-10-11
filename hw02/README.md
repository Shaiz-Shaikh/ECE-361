# Homework 2: Readings Refactored into Modules

## Description

`readings` reads sensor readings from standard input (one `tick temperature humidity` line each) and prints a summary: the number of readings, min/max/mean of temperature and humidity, the longest run above a temperature threshold, and a histogram of temperatures. The starter was one file with global variables. It is now split into modules, and the output is unchanged byte for byte.

## Build and test

Build the program `./readings`:

    make

Build and run the unit tests, then the regression test:

    make test

Remove everything the build produced:

    make clean

Run the program (the threshold in degrees C is optional, default 30.0):

    ./readings < data/normal.txt
    ./readings 25 < data/edge.txt

`make test` runs `tests/test_stats` (unit tests for the stats module), then `sh regress.sh`, which compares the program's output with the files in `expected/` using `diff`.

## Module diagram

An arrow means "calls".

                      +-------------------+
                      |      main.c       |
                      |  command line,    |
                      |  calls modules,   |
                      |  prints results   |
                      +---------+---------+
                                |
              +-----------------+-----------------+
              |                 |                 |
              v                 v                 v
     +----------------+ +----------------+ +------------------+
     |    reading     | |     stats      | |    histogram     |
     | reading.h/.c   | | stats.h/.c     | | histogram.h/.c   |
     | read and parse | | min, max, mean | | bin the temps    |
     | input lines    | | longest run    | | and print the    |
     |                | | above a        | | histogram        |
     |                | | threshold      | |                  |
     +----------------+ +----------------+ +------------------+

Only `main` calls the other modules. The three modules do not call each other.
