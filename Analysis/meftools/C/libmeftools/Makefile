#Makefile for Ncs2Mef project
CC = g++
#CFLAGS = -m64 -Wunused-variable -Wuninitialized
CFLAGS = -m64 -g -w
OPTFLAGS = -O3 
MAIN = main.c
#MEFLIB_DIR = ../MEF_3_starter_kit
MEFLIB_DIR = .
SRCFILES = convert_ncs.c write_mef_channel.c $(MEFLIB_DIR)/meflib.c $(MEFLIB_DIR)/mefrec.c
TARGET = Ncs2Mef3
#INCLUDE = ../MEF_3_starter_kit
INCLUDE = /Users/markbower/Dropbox/Documents/Concepts/2018_07_29_meftools/meftools/Analysis/meftools/inst/include


all: #full build
	$(CC) -o $(TARGET) $(CFLAGS) $(MAIN) $(SRCFILES) -I $(INCLUDE)

read_mef_header: # general purpose function
	$(CC) -c read_mef_header.cpp $(CFLAGS) -I $(INCLUDE)

VectorIterator: # general purpose function 
	$(CC) -c VectorIterator.cpp $(CFLAGS)

readMef: # general purpose function 
	$(CC) -c readMef.cpp $(CFLAGS) read_mef_header.cpp VectorIterator.o read_mef_header.o -I $(INCLUDE)

peakFinder: # application
	$(CC) -o peakFinder peakFinder.cpp $(CFLAGS) read_mef_header.o readMef.o -I $(INCLUDE)





