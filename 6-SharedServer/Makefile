CC=gcc

type-1: src/type-1/client.c src/type-1/server.c
	$(CC) -o client-1 src/type-1/client.c
	$(CC) -o server-1 src/type-1/server.c

type-2: src/type-2/client.c src/type-2/server.c
	$(CC) -o client-2 src/type-2/client.c
	$(CC) -o server-2 src/type-2/server.c

type-3: src/type-3/client.c src/type-3/server.c
	$(CC) -o client-3 src/type-3/client.c
	$(CC) -o server-3 src/type-3/server.c

.DEFAULT_GOAL := all
all: type-1 type-2 type-3

clean:
	rm -f client-1 client-2 client-3 server-1 server-2 server-3
