CC=gcc

client-removes-memory:
	$(CC) -DCLIENT_REMOVES_MEMORY src/client.c -o client-1 -lrt 
	$(CC) src/server.c -o server-1 -lrt

server-removes-memory:
	$(CC) src/client.c -o client-2 -lrt 
	$(CC) -DSERVER_REMOVES_MEMORY src/server.c -o server-2 -lrt

.DEFAULT_GOAL := all
all: server-remove-memory client-remove-memory

clean:
	rm -f client-1 client-2 server-1 server-2