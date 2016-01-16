CC = gcc

CFLAGS = -Wall -g
LIB = -pthread

all: dogwashtest

dogwashtest: dogwashtest.c dogwashsynch.h dogwashsynch.c
	$(CC) $(CFLAGS) $(LIB) dogwashtest.c dogwashsynch.c -o dogwash.o

clean:
	rm -f *.o