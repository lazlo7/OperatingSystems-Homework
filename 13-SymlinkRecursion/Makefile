CC=gcc

prog: prog.c
	$(CC) prog.c -o prog

all: prog
.DEFAULT_GOAL := all

# Remove all symlinks in ./out/, remove the original file '1', keep '.gitignore' 
cleanout:
	find out -maxdepth 1 -type l -delete
	rm -f ./out/1

clean: cleanout
	rm -f prog