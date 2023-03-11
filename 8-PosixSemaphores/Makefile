CC=gcc
LIBS=-lrt -pthread 
DEPS=src/cycle_buffer.c src/common.c

reader: $(DEPS) src/reader.c
	$(CC) $(DEPS) src/reader.c -o reader $(LIBS)

writer: $(DEPS) src/writer.c
	$(CC) $(DEPS) src/writer.c -o writer $(LIBS)

all: reader writer
.DEFAULT_GOAL := all

clean:
	rm -f reader writer