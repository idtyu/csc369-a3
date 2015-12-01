#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ext2_lib.h"

int main(int argc, char *argv[]) {
	if(argc != 4) {
        fprintf(stderr, "Usage: ext2_ln <image file name> <source> <destination>\n");
        exit(1);
    }
	return 1;	
}