#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ext2_lib.h"

int main(int argc, char **argv){   
        if(argc != 2) {
            fprintf(stderr, "Usage: ext2_ls <image file name> <directory>\n");
            exit(1);
        }
        fd = open(argv[1], O_RDWR);
        disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);            
        if(disk == MAP_FAILED) {
            perror("mmap");
	    exit(1);
        }
        init();
	return 0;
}