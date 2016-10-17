###############################################################################
#
# Makefile to produce a program to generate (random) matrices to disk and a
# library to read and write the matrices
#
# Group 5: Grigorii, Jorge, Xiukun
#
# How to use this: from a terminal or command line window:
#
#   make                Compile and link all of the programs.
#   make matrix		Create matrix to calculate.
#   make run            Run the multiplication in parallel.
#   make runserial	Run the multiplication in serial.
#   make clean          Remove files related to making the example program.
#   make matrixclean    Remove the blocks of the matrices and the description
#                       of the matrix and block sizes.
#   make distclean      Try to get back to what the directory looked like
#                       when it was first created.
#
###############################################################################

SHELL = /bin/sh
PROJECT = G5hw1

MATRIX = mkmatrices
TARGET = MMmultiple
STARGET= MMmultiple_serial

CFLAGS = -g
LFLAGS = -g -O3
OMPFLAGS = -fopenmp

CC     = gcc

TFILE = $(PROJECT).tgz
ZFILE = $(PROJECT).zip
PACKFILES = Makefile matrices.h matrices.c mkmatrices.c mkmatrices.h MMmultiple_parallel.c timer.c report.pdf


all: 	$(MATRIX) $(TARGET) $(STARGET)

matrix: $(MATRIX)
	- ./$(MATRIX)
	
parallel: $(TARGET)

serial:	$(STARGET)

runserial:	$(STARGET)
	- ./$(STARGET)

run: 	parallel
	- ./$(TARGET)
	
$(TARGET):	$(TARGET).o matrices.o timer.o Makefile
	$(CC) $(LFLAGS) $(OMPFLAGS) -o $(TARGET) $(TARGET).o matrices.o timer.o

$(STARGET):	$(STARGET).o matrices.o timer.o Makefile
	$(CC) $(LFLAGS) -o $(STARGET) $(STARGET).o matrices.o timer.o

$(MATRIX): 	$(MATRIX).o timer.o matrices.o

$(TARGET).o: 	MMmultiple.c matrices.h Makefile
	$(CC) $(LFLAGS) -c $(OMPFLAGS) MMmultiple.c

$(STARGET).o:	MMmultiple.c matrices.h Makefile
	$(CC) $(LFLAGS) -o $(STARGET).o -c MMmultiple.c

matrices.o:	matrices.c matrices.h Makefile

mkmatrices.o:	mkmatrices.c matrices.h Makefile


clean:
	- /bin/rm -f *.o $(TARGET) $(TFILE) $(ZFILE) $(STARGET)

matrixclean:
	- /bin/rm -f matrices.txt A*.bin B*.bin C*.bin

distclean:	clean matrixclean

pack:
	tar zcvf $(TFILE) $(PACKFILES)
	zip -r $(ZFILE) $(PACKFILES)

CC:
	echo $(CC)

###############################################################################
#
# C'est finis!
#
###############################################################################
