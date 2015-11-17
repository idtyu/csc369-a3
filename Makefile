CC=gcc
CFLAGS= -Wall -g -o
LIBFLAGS= -Wall -g -c
DEP= ext2_lib.h

all:  ext2_lib.o ext2_ls ext2_cp ext2_mkdir ext2_ln ext2_rm

ext2_ls: ext2_lib.o $(DEP)
	$(CC) $(CFLAGS) $@ $^

ext2_cp: ext2_lib.o $(DEP)
	$(CC) $(CFLAGS) $@ $^

ext2_mkdir: ext2_lib.o $(DEP)
	$(CC) $(CFLAGS) $@ $^

ext2_ln: ext2_lib.o $(DEP)
	$(CC) $(CFLAGS) $@ $^

ext2_rm: ext2_lib.o $(DEP)
	$(CC) $(CFLAGS) $@ $^

ext2_lib.o: ext2_lib.c $(DEP)
	$(CC) $(LIBFLAGS) $<


clean:
	find . -not -name '*.c' -not -name '*.h' -not -name 'Makefile' -not -path './.git/*' -type f | xargs rm -r
