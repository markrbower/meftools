#Makefile for Ncs2Mef project
CC = gcc
#CFLAGS = -m64 -Wunused-variable -Wuninitialized
CFLAGS = -m64 -g
OPTFLAGS = -O3 
MAIN = main.c
#MEFLIB_DIR = ../MEF_3_starter_kit
MEFLIB_DIR = .
SRCFILES = convert_ncs.c write_mef_channel.c $(MEFLIB_DIR)/meflib.c $(MEFLIB_DIR)/mefrec.c
TARGET = Ncs2Mef3
#INCLUDE = ../MEF_3_starter_kit
INCLUDE = .


all: #full build
	$(CC) -o $(TARGET) $(CFLAGS) $(MAIN) $(SRCFILES) -I $(INCLUDE)

mef2: 
	$(CC) -o Ncs2Mef2 $(CFLAGS) -DOUTPUT_TO_MEF2 main.c convert_ncs.c write_mef_channel_mef2.c mef_lib.c
