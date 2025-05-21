# Project: Windows example
# Makefile created by Dev-C++ 5.11

CPP      = g++
CC       = gcc
OBJ      = main.o $(RES)
LINKOBJ  = main.o $(RES)

LIBS     =
INCS     =
BIN      = PHCK7Tool.exe
CXXFLAGS = $(CXXINCS) 
CFLAGS   = $(INCS) 
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)


