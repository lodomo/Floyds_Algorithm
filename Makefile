###############################################################################
#
# Author: Lorenzo Moon	
# Description: Basic Makefile for a C program
# Targets: all (default)
# 		   clean           : remove all generated files
# 		   tar             : create a tarball of the files
# 		   git             : add, commit, and push to git
# 		   debug           : compile with debug flags
# 		   test            : run test-PROGRAM.bash
#
###############################################################################

PROGRAM = floyds
CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wunreachable-code \
         -Wredundant-decls -Wmissing-declarations \
         -Wold-style-definition -Wmissing-prototypes \
         -Wdeclaration-after-statement -Wno-return-local-addr \
         -Wunsafe-loop-optimizations -Wuninitialized -Werror \
         -Wno-unused-parameter -Wno-string-compare -Wno-stringop-overflow \
         -Wno-stringop-overread -Wno-stringop-truncation
DFLAGS = -g
LDFLAGS = # Add any linker flags here if needed
TAR_FILE = ${PROGRAM}.tar.gz
TEST_FILE = test-${PROGRAM}.bash

all: $(PROGRAM)

$(PROGRAM): $(PROGRAM).o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) -c $<

clean clena claen cls cl:
	rm -f $(PROGRAM) $(PROGRAM).o *~ \#*
	rm -f *.output *.error

tar:
	tar -cvaf ${TAR_FILE} *.[ch] [Mm]akefile

git:
	git add .
	git commit -m "Lazy commit via Make"
	git push

test:
	./$(TEST_FILE)

debug: 
	$(CC) $(CFLAGS) $(DFLAGS) -o $(PROGRAM) $(PROGRAM).c $(LDFLAGS)
