CC=gcc
CFLAGS= -Wall -g -o
LIBFLAGS= -Wall -g -c
DEP= ext2_lib.h ext2.h

all:  ext2_ls

ext2_ls: ext2_lib.c ext2_ls.c
	gcc -o ext2_ls ext2_ls.c ext2_lib.c

clean:
	find . -not -name '*.c' -not -name '*.h' -not -name 'Makefile' -not -path './.git/*' -type f | xargs rm -r
