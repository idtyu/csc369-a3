#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ext2_lib.h>
#include <ext2_lib.c>
#include <ext2.h>


int main(int argc, char *argv[]) {
	if(argc != 3) {
        fprintf(stderr, "Usage: ext2_mkdir <image file name> <directory name>\n");
        exit(1);
    }
	return 1;	
}