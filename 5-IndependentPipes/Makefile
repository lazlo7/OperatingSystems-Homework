CC=gcc

writer: writer.c
	$(CC) -o writer writer.c

reader: reader.c
	$(CC) -o reader reader.c

.DEFAULT_GOAL := all
all: writer reader

clean:
	rm -f writer reader