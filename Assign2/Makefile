#Keenan Johnstone 	- 11119412 	- kbj182
#Kyle Sheasby		- 11112400 	- kms123

CFLAGS = -Wall

.PHONY: all clean

all: Test.o lib_mem.a 

Test.o: Memory_Allocator.c
	gcc -g $(CFLAGS) -o Test.o Memory_Allocator.c

lib_mem.a: Test.o
	ar -rv lib_mem.a Test.o

clean:
	$(RM) Test.o
	$(RM) lib_mem.a
