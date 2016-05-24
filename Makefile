CC = gcc -Wall -O1 -g
LDFLAGS = -lpthread

all: server client

server: server.c lib.h socket_util.c socket_util.h showcase_util.c showcase_util.h thread.c thread.h
	$(CC) -o server server.c thread.c showcase_util.c socket_util.c $(LDFLAGS)

client: client.c lib.h socket_util.c socket_util.h showcase_util.c showcase_util.h
	$(CC) -o client client.c showcase_util.c socket_util.c $(LDFLAGS)

:phony

run:
	./server

clean:
	rm -f client server
