#Makefile for Ncs2Mef project
CC = g++
#CFLAGS = -m64 -Wunused-variable -Wuninitialized
#CFLAGS = -m64 -g -std=c++17 
CFLAGS = -m64 -g -w -std=c++17 -Wall -Wc++17-extensions
OPTFLAGS = -O3 
MAIN = main.c
#MEFLIB_DIR = ../MEF_3_starter_kit
MEFLIB_DIR = .
SRCFILES = convert_ncs.c write_mef_channel.c $(MEFLIB_DIR)/meflib.c $(MEFLIB_DIR)/mefrec.c
TARGET = Ncs2Mef3
#INCLUDE = ../MEF_3_starter_kit
INCLUDE = /Users/markbower/Dropbox/Documents/Concepts/2018_07_29_meftools/meftools/Analysis/meftools/inst/include
INCLUDE_MYSQLX = /opt/homebrew/Cellar/mysql-connector-c++/9.6.0/include
INCLUDE_MYSQL = /opt/homebrew/opt/mysql-client/include
INCLUDE_FILTFILT = /Users/markbower/Dropbox/Documents/src/KBaur/FiltFilt/src/include

USER_OBJS = $(wildcard /Users/markbower/Dropbox/Documents/Concepts/2018_07_29_meftools/meftools/Analysis/meftools/C/libmeftools/*.o)


all: #full build
	$(CC) -o $(TARGET) $(CFLAGS) $(MAIN) $(SRCFILES) -I $(INCLUDE)

CircularBuffer:
	$(CC) -c CircularBuffer.cpp $(CFLAGS) -I $(INCLUDE)

CircularBufferMEF:
	$(CC) -c CircularBufferMEF.cpp $(CFLAGS) -I $(INCLUDE)

CircularBufferMEF_allPeaks:
	$(CC) -c CircularBufferMEF_allPeaks.cpp $(CFLAGS) -I $(INCLUDE)

CircularBufferMEF_P13:
	$(CC) -c CircularBufferMEF_P13.cpp $(CFLAGS) -I $(INCLUDE)

CircularBufferMEF:
	$(CC) -c CircularBufferMEF.cpp $(CFLAGS) -I $(INCLUDE)

CircularBufferMEF:
	$(CC) -c CircularBufferMEF.cpp $(CFLAGS) -I $(INCLUDE)

Filt:
	$(CC) -c kbaur/Filt.cpp $(CFLAGS) -o kbaur/Filt.o

FiltFilt:
	$(CC) -c kbaur/FiltFilt.cpp $(CFLAGS) -o kbaur/FiltFilt.o

read_mef_header: # general purpose function
	$(CC) -c read_mef_header.cpp $(CFLAGS) -I $(INCLUDE)

table_of_contents:
	$(CC) -c table_of_contents.cpp $(CFLAGS) -I $(INCLUDE)

get_discontinuities:
	$(CC) -c get_discontinuities.cpp $(CFLAGS) -I $(INCLUDE)

decomp_mef:
	$(CC) -c decomp_mef.cpp $(CFLAGS) -I $(INCLUDE)

MEFinfo:
	$(CC) -c MEFinfo.cpp $(CFLAGS) -I $(INCLUDE)

MEFiter: # general purpose function 
	$(CC) -c MEFiter.cpp $(CFLAGS) -I $(INCLUDE) MEFinfo.o

MEFconts: # general purpose function 
	$(CC) -c MEFconts.cpp $(CFLAGS) -I $(INCLUDE)

VectorIterator: # general purpose function
	$(CC) -c VectorIterator.cpp $(CFLAGS) -I $(INCLUDE)

MEFanalysis: # general purpose function 
	$(CC) -c MEFanalysis.cpp $(CFLAGS) -I $(INCLUDE) -I $(INCLUDE_MYSQLX) -I $(INCLUDE_MYSQL)

#analysisFindPeaks: # general purpose function 
#	$(CC) -c analysisFindPeaks.cpp  -I $(INCLUDE) -I $(INCLUDE_MYSQL) -I $(INCLUDE_MYSQLX) -I $(INCLUDE_FILTFILT) -L/opt/homebrew/Cellar/mysql-connector-c++/9.6.0/lib -lmysqlcppconnx.2.9.6.0 -L/usr/local/mysql-9.6.0-macos15-arm64/lib -lmysqlclient -Wl,-rpath,/usr/local/mysql/lib

analysisFindPeaks: # general purpose function 
	$(CC) -c analysisFindPeaks.cpp  $(CFLAGS) -I $(INCLUDE) -I $(INCLUDE_MYSQL) -I $(INCLUDE_MYSQLX) -I $(INCLUDE_FILTFILT)

analysisNPI: # general purpose function 
	$(CC) -c analysisNPI.cpp $(CFLAGS) -I $(INCLUDE) -I $(INCLUDE_MYSQL) MEFiter.o

da: # general purpose function 
	$(CC) -c DatabaseAccessor.cpp $(CFLAGS) -I $(INCLUDE) -I $(INCLUDE_MYSQL) -I $(INCLUDE_MYSQLX) -L/opt/homebrew/Cellar/mysql-connector-c++/9.6.0/lib -lmysqlcppconnx.2.9.6.0  -L/usr/local/mysql-9.6.0-macos15-arm64/lib -lmysqlclient -Wl,-rpath,/usr/local/mysql/lib



processMEFexample: # application
	$(CC) -c processMEFexample.cpp $(CFLAGS) -I $(INCLUDE) -I $(INCLUDE_MYSQLX) -I $(INCLUDE_MYSQL) -I $(INCLUDE_FILTFILT)

main: # analysisLookingForPeaks
	$(CC) -o main.cpp $(CFLAGS) -I $(INCLUDE) processMEFexample.o

main_alfp: # analysisLookingForPeaks
	$(CC) main_analysisLookingForPeaks.cpp -o main $(CFLAGS) -I $(INCLUDE) -I $(INCLUDE_MYSQL) -I $(INCLUDE_MYSQLX) -I $(INCLUDE_FILTFILT) $(USER_OBJS) -L/opt/homebrew/Cellar/mysql-connector-c++/9.6.0/lib -lmysqlcppconnx.2.9.6.0 kbaur/Filt.o kbaur/FiltFilt.o -L/usr/local/mysql-9.6.0-macos15-arm64/lib -lmysqlclient -Wl,-rpath,/usr/local/mysql/lib

da_test: # test DatabaseAccessor
	$(CC) DatabaseAccessor_test.cpp -o DatabaseAccessor_test $(CFLAGS) -I $(INCLUDE) -I $(INCLUDE_MYSQL) -I $(INCLUDE_MYSQLX) -L/opt/homebrew/Cellar/mysql-connector-c++/9.6.0/lib -lmysqlcppconnx.2.9.6.0 DatabaseAccessor.o -L/usr/local/mysql-9.6.0-macos15-arm64/lib -lmysqlclient -Wl,-rpath,/usr/local/mysql/lib



