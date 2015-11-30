#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "ext2_lib.h"

void init(char* dir){
    fd = open(dir, O_RDWR);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
	    exit(1);
    }

    super_block = (struct ext2_super_block *)(disk + 1024);
    group_desc = (struct ext2_group_desc*)READ_ONE_BLOCK(super_block->s_first_data_block);
    getbitmap();
    read_inode_table_n_blocks();
}

int bitmapToBlock(char* bitmap, unsigned int index){
    unsigned int arrayPosition = index / 8 ;
    unsigned int shiftPosition = index % 8;
    return (bitmap[arrayPosition] >> shiftPosition) & 1;
}

void read_inode_table_n_blocks(){
   int i;
   inode_table = (struct ext2_inode*)READ_ONE_BLOCK(group_desc->bg_inode_table);
   int count = super_block ->s_inodes_count;
   for (i = 0; i < count; i++){   
        if (bitmapToBlock(inode_bitmap,i )){ // not empty
            unsigned short mode = inode_table[i].i_mode;
            char *type = malloc(sizeof(char));
            if (mode & EXT2_S_IFREG){
                strcat(type,"f"); 
            }if (mode & EXT2_S_IFDIR){
                strcat(type,"d");
            }
            printf("[%d] type: %s size: %d links: %d blocks: %d \n",i+1,type,inode_table[i].i_size,inode_table[i].i_links_count,
            inode_table[i].i_blocks);
            printf("[%d] Blocks:  ",i+1);
            int idx;
            for(idx = 0; idx < inode_table[i].i_blocks+1; idx++)
            {
                int num = inode_table[i].i_block[idx];
                if (num == 0){
                    break;
                }else{
                    
                    block_table[idx+i] = *(struct ext2_dir_entry_2*)(inode_table[i].i_block+idx);
                }
                printf("%d",num);
            }
            printf("\n");
        }
    }
}

/*for both inode and block bitmaps*/
void getbitmap(char* data){
    inode_bitmap = (char*)READ_ONE_BLOCK(group_desc->bg_inode_bitmap);
    block_bitmap = (char*)READ_ONE_BLOCK(group_desc->bg_block_bitmap);
}