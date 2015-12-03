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

struct ext2_inode* inode_table;

void init(char* filename){    
    fd = open(filename, O_RDWR);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);            
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
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
}

int bitmapToBlock(char* bitmap, unsigned int index){
    unsigned char mask[] = {1, 2, 4, 8, 16, 32, 64, 128};
    unsigned int arrayPosition = index / 8 ;
    unsigned int shiftPosition = index % 8;
    return ((bitmap[arrayPosition] & mask[shiftPosition])!=0);
}


/*
void read_inode_table_n_blocks(){
   int i;
   inode_tables = (struct ext2_inode*)(disk + (group_desc->bg_inode_table * EXT2_BLOCK_SIZE));
   int count = super_block->s_inodes_count;
   for (i = 0; i < count; i++){
        int inode_num = super_block -> s_inodes_count;
    int i;
    for(i=0;i<inode_num;i++){
        if(i==1){
            struct ext2_inode inode_table = inode_tables[i];
            int j;
            for(j=0;j<inode_table.i_blocks;j++){
                block_tables =(struct ext2_dir_entry_2*) &inode_table.i_block[j];
                
            }
        }else if(bitmapToBlock(inode_bitmap,i) && i >= 11 ){
            struct ext2_inode inode_table = inode_tables[i];
            int j;
            for(j=0;j<inode_table.i_blocks;j++){
                struct ext2_dir_entry_2*  block_table =  (struct ext2_dir_entry_2*)(& inode_table.i_block[j]);
               printf("[%d]file name: %s \n",i,block_table->name);
                if(strcmp(block_table->name,fileName)){
                    return 1;
                }
            }
        }
   }
}
*/
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
