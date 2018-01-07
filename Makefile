BIN=bfc
CXX=g++
LIBS=-lboost_program_options -lboost_system -lboost_filesystem
CFLAGS=-Wall -std=c++11

all: $(BIN)

include make.deps

$(BIN): bfc.o options.o program.o file.o
	$(CXX) -o $@ $^ $(LIBS) $(CFLAGS)

.cpp.o:
	$(CXX) -c -o $@ $< $(CFLAGS)

clean:
	rm *.o

.PHONY: all clean
