# Makefile for mre_twoLevels project
# Can I have B persistently manage it's state in multiple ways? 
#
# If main calls B within A when B=NULL, have B init itself, and have that persist outside of B?
#
CFLAGS = -m64 -g -w -std=c++20 -Wall
OPTFLAGS = -O3 

USER_OBJS = $(wildcard /Users/markbower/Dropbox/Documents/Concepts/2018_07_29_meftools/meftools/Analysis/meftools/C/libmeftools/mre_twoLevels/*.o)


A: #full build
	$(CC) A.cpp -c $(CFLAGS)

B: #full build
	$(CC) B.cpp -c $(CFLAGS)

main: A B #full build
	$(CC) main.cpp -o main $(CFLAGS) A.o B.o -lstdc++


