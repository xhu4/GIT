# HPMM - High Performance Matrix Multiplication.

**HPMM** holds the (first) homework solution code of Group 5 for Craig Douglas' _High Performance Computing_ course in 2016 Fall, University of Wyoming.

It contains several different implementation of matrix multiplication, all written in C.

## How to Run

-   `make`: Compile and link all of the programs.
-   `make matrix`: Create matrix to calculate.
-   `make run`: Run the multiplication in parallel.
-   `make runserial`: Run the multiplication in serial.
-   `make clean`: Remove files related to making the example program.
-   `make matrixclean`: Remove the blocks of the matrices and the description
                        of the matrix and block sizes.
-   `make distclean`: Try to get back to what the directory looked like
                      when it was first created.
                      
## Known Issues

- Cannot compile with clang in parallel because of unrecognized flag `-fopenmp`.
- Currently (August 2018) get segmentation fault 11.
