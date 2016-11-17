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
PROJECT = G5hw2-3

ATLAS  = atlasmm
MATRIX = mkmatrices
TARGET = MMultiple4
STARGET= MMultiple_serial

CFLAGS = -g
LFLAGS = -g -O4
OMPFLAGS = -fopenmp

CC     = gcc

LPATH = /usr/local/atlas/lib
LIBFLAGS= -lcblas -latlas
IPATH = /usr/local/atlas/include/

TFILE = $(PROJECT).tgz
ZFILE = $(PROJECT).zip
PACKFILES = Makefile matrices.h matrices.c $(MATRIX).c $(TARGET).c timer.c report.pdf


all: 	$(MATRIX) $(TARGET) $(STARGET)

matrix: $(MATRIX)
	- ./$(MATRIX)
	
parallel: $(TARGET)

serial:	$(STARGET)

atlas: $(ATLAS)
	- ./$(ATLAS)

runserial:	$(STARGET)
	- ./$(STARGET)

run: 	parallel
	-@ ./$(TARGET)
	
$(TARGET):	$(TARGET).o matrices.o timer.o Makefile
	$(CC) $(LFLAGS) $(OMPFLAGS) -o $(TARGET) $(TARGET).o matrices.o timer.o

$(STARGET):	$(STARGET).o matrices.o timer.o Makefile
	$(CC) $(LFLAGS) -o $(STARGET) $(STARGET).o matrices.o timer.o

$(MATRIX): 	$(MATRIX).o timer.o matrices.o

$(ATLAS):	$(ATLAS).c timer.o Makefile
	$(CC) $(ATLAS).c timer.o -L$(LPATH) $(LIBFLAGS) -I$(IPATH) -o $(ATLAS)

$(TARGET).o: 	$(TARGET).c matrices.h Makefile
	$(CC) $(LFLAGS) -c $(OMPFLAGS) $(TARGET).c

$(STARGET).o:	$(TARGET).c matrices.h Makefile
	$(CC) $(LFLAGS) -o $(STARGET).o -c $(TARGET).c

matrices.o:	matrices.c matrices.h Makefile

mkmatrices.o:	mkmatrices.c matrices.h Makefile


clean:
	- /bin/rm -f *.o $(TARGET) $(TFILE) $(ZFILE) $(STARGET) MMultiple MMultiple4 MMultiple2 $(MATRIX) $(STARGET)

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
