
SHELL = /bin/sh

MAIN_NAME =vmas
SUBA_NAME =vmass

MAIN_SRC = ./src
MAIN_OUT = ./bin
MAIN_DAT = ./dat
MAIN_EXE = $(MAIN_OUT)/$(MAIN_NAME)

SUBA_EXE = $(MAIN_OUT)/$(SUBA_NAME)

HLPR_TWEAK = tweak

CFLAGS = 

CC = gcc $(CFLAGS)


all: $(MAIN_EXE) $(SUBA_EXE)
	echo 'Done and done.'

	
$(MAIN_EXE): nvmas.c nvmaslib.c
	$(CC) -o $(MAIN_EXE) nvmas.c nvmaslib.c
	
$(SUBA_EXE): nvmass.c
	$(CC) -o $(SUBA_EXE) nvmass.c
	


run:
	$(MAIN_EXE) <$(MAIN_DAT)/mbr.masb
	
newdata: config
	$(SUBA_EXE) $(MAIN_DAT)/mbr.masb
	
valdata: 
	$(HLPR_TWEAK) $(MAIN_DAT)/mbr.masb
	
config: 
	mkdir -p $(MAIN_OUT)
	mkdir -p $(MAIN_DAT)




