TARGET=bfc
LDLIBS=-lboost_program_options -lboost_system -lboost_filesystem -lpthread

all: $(TARGET)

include make.deps

$(TARGET): bfc.o options.o program.o file.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) *.o $(TARGET)

.PHONY: all clean
