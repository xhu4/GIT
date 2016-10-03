###############################################################################
#
# Makefile to produce a program to generate (random) matrices to disk and a
# library to read and write the matrices
#
# Craig C. Douglas (ccd)
#
# History of modification:
#   Wed Jun 22 17:54:43 AST 2011  ccd @ kaust
#   Fri Jun 24 16:12:20 AST 2011  ccd @ kaust
#
# How to use this: from a terminal or command line window:
#
#   make                Compile and link all of the programs.
#   make run            Like make, but also run the example program.
#   make clean          Remove files related to making the example program.
#   make matrixclean    Remove the blocks of the matrices and the description
#                       of the matrix and block sizes.
#   make distclean      Try to get back to what the directory looked like
#                       when it was first created.
#
###############################################################################

SHELL = /bin/sh
PROJECT = G5hw1

STARGET = MMmultiple
TARGET = MMmultiple_parallel
CFLAGS = -g
LFLAGS = -g 
OMPFLAGS  = -fopenmp
CC     = gcc-6

TFILE = $(PROJECT).tgz
ZFILE = $(PROJECT).zip
PACKFILES = Makefile matrices.h matrices.c mkmatrices.c mkmatrices.h MMmultiple_parallel.c timer.c report.pdf

all: 	$(TARGET) $(STARGET)

parallel: $(TARGET)

serial:	$(STARGET)

runserial:	$(STARGET)
	- ./$(STARGET)

run: 	parallel
	- ./$(TARGET)
	
$(TARGET):	$(TARGET).o mkmatrices.o matrices.o timer.o Makefile
	$(CC) $(LFLAGS) $(OMPFLAGS) -o $(TARGET) $(TARGET).o mkmatrices.o matrices.o timer.o

$(STARGET):	$(STARGET).o mkmatrices.o matrices.o timer.o Makefile
	$(CC) $(LFLAGS) -o $(STARGET) $(STARGET).o mkmatrices.o matrices.o timer.o

MMmultiple_parallel.o: matrices.h mkmatrices.h Makefile
	$(CC) $(LFLAGS) -c $(OMPFLAGS) MMmultiple_parallel.c

MMmultiple.o: 	mkmatrices.h Makefile
	$(CC) $(LFLAGS) -c MMmultiple.c

matrices.o:	matrices.h Makefile
mkmatrices.o:	mkmatrices.h matrices.h Makefile


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
