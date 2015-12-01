CC=gcc
CFLAGS= -Wall -g
LIBFLAGS= -Wall -c

all:  ext2_lib ext2_ls ext2_cp ext2_mkdir ext2_ln ext2_rm

ext2_ls: ext2_lib.o
	$(CC) $(CFLAGS) $@.c -o $@  $^

ext2_cp: ext2_lib
	$(CC) $(CFLAGS) $@.c -o  $^.o

ext2_mkdir: ext2_lib
	$(CC) $(CFLAGS) $@.c -o  $^.o

ext2_ln: ext2_lib
	$(CC) $(CFLAGS) $@.c -o  $^.o

ext2_rm: ext2_lib
	$(CC) $(CFLAGS) $@.c -o  $^.o

ext2_lib:
	$(CC) $(LIBFLAGS) $@.c $^


clean:
	find . -not -name '*.c' -not -name '*.h' -not -name '*.img' -not -name 'Makefile' -not -path './.git/*' -not -path 'nbproject' -type f | xargs rm -r
