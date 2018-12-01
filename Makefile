CFLAGS = -Wall -Wextra

bfc: bfc.o compile.o
bfc.o: bfc.c common.h compile.h
compile.o: compile.c common.h compile.h

clean:
	$(RM) *.o bfc

.PHONY: clean
