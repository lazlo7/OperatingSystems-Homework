CC=gcc

broadcast-client: src/broadcast/client.c src/common.h
	$(CC) src/broadcast/client.c -o broadcast-client

broadcast-server: src/broadcast/server.c src/common.h
	$(CC) src/broadcast/server.c -o broadcast-server

multicast-client: src/multicast/client.c src/common.h
	$(CC) src/multicast/client.c -o multicast-client

multicast-server: src/multicast/server.c src/common.h
	$(CC) src/multicast/server.c -o multicast-server

broadcast: broadcast-client broadcast-server 
multicast: multicast-client multicast-server

all: broadcast multicast
.DEFAULT_GOAL := all

clean:
	rm -f broadcast-client broadcast-server multicast-client multicast-server
