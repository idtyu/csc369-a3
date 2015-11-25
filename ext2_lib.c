#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ext2_lib.h>
#include <ext2.h>

void init(char* dir){
	fd = open(dir, O_RDWR);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
	    exit(1);
    }

    super_block = (struct ext2_super_block *)(disk + 1024);
    int desc_offset = sb->s_first_data_block+EXT2_BLOCK_SIZE-1;
    block_desc[0] = (struct ext2_group_desc*)(disk+desc_offset);
}

/*for both inode and block bitmaps*/
char* getbitmap(char* data){
    return NULL;
}