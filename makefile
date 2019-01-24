
SHELL = /bin/sh

MAIN_NAME =vmas
SUBA_NAME =vmass

MAIN_SRC = .
MAIN_OUT = .
MAIN_DAT = .
MAIN_EXE = $(MAIN_OUT)/$(MAIN_NAME).exe
SUBA_EXE = $(MAIN_OUT)/$(SUBA_NAME).exe

# HLPR_TWEAK = tweak

CCUTIL = clang
CFLAGS = -o $(MAIN_EXE)
LFLAGS = -Xlinker\
Ws2_32.lib
CC = $(CCUTIL) $(CFLAGS) $(LFLAGS)

$(MAIN_EXE): nvmas.c nvmaslib.c
	$(CC) nvmas.c nvmaslib.c
	
$(SUBA_EXE): nvmass.c
	$(CC) -o $(SUBA_EXE) nvmass.c
	
run:
	$(MAIN_EXE) <$(MAIN_DAT)/a.out
	
newdata: config
	$(SUBA_EXE) $(MAIN_DAT)/a.out
	
valdata: 
	$(HLPR_TWEAK) $(MAIN_DAT)/a.out
	
config: 
	mkdir -p $(MAIN_OUT)
	mkdir -p $(MAIN_DAT)




