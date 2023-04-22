CC=gcc

client: src/client.c
	$(CC) src/client.c -o client

server:	src/server.c
	$(CC) src/server.c -o server

all: client server
.DEFAULT_GOAL := all

clean:
	rm -f client server
