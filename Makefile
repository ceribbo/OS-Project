CC = gcc -Wall -O1 -g
LDFLAGS =

all: server client

server: server.c lib.h socket_util.c socket_util.h showcase_util.c showcase_util.h
	$(CC) -o server server.c showcase_util.c socket_util.c $(LDFLAGS)

client: client.c lib.h socket_util.c socket_util.h showcase_util.c showcase_util.h
	$(CC) -o client client.c showcase_util.c socket_util.c $(LDFLAGS)

:phony

run:
	./server

clean:
	rm -f client server
