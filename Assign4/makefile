
CC=gcc

CFLAGS=-Wall -g -pedantic -Werror
LIB=-lpthread -pthread

all: server sender receiver

server: server.c
	$(CC) $(CFLAGS) $(LIB) server.c -o server

sender: clientSender.c
	$(CC) $(CFLAGS) $(LIB) clientSender.c -o sender

receiver: clientReceiver.c
	$(CC) $(CFLAGS) $(LIB) clientReceiver.c -o receiver

#Cleanup.
clean:
	rm -f *.o server sender receiver