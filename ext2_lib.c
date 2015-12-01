#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "ext2_lib.h"

void init(){    
    super_block = (struct ext2_super_block *)(disk + 1024);
    int desc_offset = super_block->s_first_data_block+2*EXT2_BLOCK_SIZE-1;
    group_desc = (struct ext2_group_desc*)(disk+desc_offset);
    printf("Inodes: %d\n", super_block->s_inodes_count);
    printf("Blocks: %d\n", super_block->s_blocks_count);
    printf("Block group: \n");
    printf("\t block bitmap: %d \n",group_desc->bg_block_bitmap);
    printf("\t inode bitmap: %d \n",group_desc->bg_inode_bitmap);
    printf("\t inode tables: %d \n", group_desc->bg_inode_table);
    printf("\t free blocks:%d \n",group_desc->bg_free_blocks_count);
    printf("\t free inodes:%d \n",group_desc->bg_free_inodes_count);
    printf("\t user dirs: %d \n",group_desc->bg_used_dirs_count);
    getbitmap();
    read_inode_table_n_blocks();
}

int bitmapToBlock(char* bitmap, unsigned int index){
    unsigned int arrayPosition = index / 8 ;
    unsigned int shiftPosition = index % 8;
    unsigned int shifted = bitmap[arrayPosition] >> shiftPosition;
    return ((shifted & 1) == shifted);
}

void read_inode_table_n_blocks(){
   int i;
   inode_table = (struct ext2_inode*)READ_ONE_BLOCK(group_desc->bg_inode_table);
   int count = super_block ->s_inodes_count;
   for (i = 0; i < count; i++){
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

/*for both inode and block bitmaps*/
void getbitmap(){
    if(group_desc!=NULL && disk!=NULL){
        inode_bitmap = (char*)READ_ONE_BLOCK(group_desc->bg_inode_bitmap);
        block_bitmap = (char*)READ_ONE_BLOCK(group_desc->bg_block_bitmap);
        int i;
        printf("block bitmap:");
        for(i=0;i<super_block->s_blocks_count/8;i++){
             printBits(block_bitmap[i]);
        }
        printf("\n");
        printf("inode bitmap:");
        for(i=0;i<(super_block->s_inodes_count/8);i++){
            printBits(inode_bitmap[i]);
        }
        printf("\n");
    }
}

void printBits(unsigned char block){
    int i;
    for(i=0;i<8;i++){
        printf("%d",(block >> i) & 1);
    }
    printf(" ");
}
