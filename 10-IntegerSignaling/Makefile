CC=gcc

sender: src/sender.c
	$(CC) -o sender src/sender.c

receiver: src/receiver.c
	$(CC) -o receiver src/receiver.c

all: sender receiver
.DEFAULT_GOAL := all

clean:
	rm -f sender receiver
