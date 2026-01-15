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

table_of_contents:
	$(CC) -c table_of_contents.cpp $(CFLAGS) -I $(INCLUDE)

get_discontinuities:
	$(CC) -c get_discontinuities.cpp $(CFLAGS) -I $(INCLUDE)

MEFinfo:
	$(CC) -c MEFinfo.cpp $(CFLAGS) -I $(INCLUDE) table_of_contents.o get_discontinuities.o

MEFiter: # general purpose function 
	$(CC) -c MEFiter.cpp $(CFLAGS) -I $(INCLUDE) MEFinfo.o

MEFcont: # general purpose function 
	$(CC) -c MEFcont.cpp $(CFLAGS) -I $(INCLUDE) MEFinfo.o MEFiter.o VectorIterator.o

VectorIterator: # general purpose function 
	$(CC) -c VectorIterator.cpp $(CFLAGS) -I $(INCLUDE)

processMEFexample: # application
	$(CC) -c processMEFexample.cpp $(CFLAGS) -I $(INCLUDE) MEFinfo.o MEFiter.o MEFcont.o

main: # analysisLookingForPeaks
	$(CC) -o main.cpp $(CFLAGS) -I $(INCLUDE) processMEFexample.o



