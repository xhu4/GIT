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
PROJECT = hw1

TARGET = MMmultiple
CFLAGS = -g
LFLAGS = -g

TFILE = $(PROJECT).tgz
ZFILE = $(PROJECT).zip
PACKFILES = Makefile matrices.h matrices.c mkmatrices.c mkmatrices.h MMmultiple.c timer.c

all:	$(TARGET)

run:	all
	- ./$(TARGET)

MMmultiple.o: 	mkmatrices.c mkmatrices.h Makefile
matrices.o:	matrices.c matrices.h Makefile
mkmatrices.o:	mkmatrices.c mkmatrices.h matrices.h Makefile
timer.o:	timer.c

$(TARGET):	$(TARGET).o mkmatrices.o matrices.o timer.o Makefile
	$(CC) $(LFLAGS) -o $(TARGET) $(TARGET).o mkmatrices.o matrices.o timer.o

clean:
	- /bin/rm -f *.o $(TARGET) $(TFILE) $(ZFILE)

matrixclean:
	- /bin/rm -f matrices.txt A*.bin B*.bin C*.bin

distclean:	clean matrixclean

pack:
	tar zcvf $(TFILE) $(PACKFILES)
	zip -r $(ZFILE) $(PACKFILES)

###############################################################################
#
# C'est finis!
#
###############################################################################
