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

struct inode_dir_pair fileExists(char* matchName) {
    int inode_num = super_block -> s_inodes_count;
    int i;
    newPair.inode_number = -1;
    for (i = 0; i < inode_num; i++) {
        if ((i == 1) || (bitmapToBlock(inode_bitmap, i) && i >= 11)) {
            struct ext2_inode inode_table = inode_tables[i];
            int j;
            for (j = 0; j < inode_table.i_blocks; j++) {
                 
                struct ext2_dir_entry_2* block_table;
                if (inode_table.i_block[j] == 0) {
                    break;
                }
                block_table = (struct ext2_dir_entry_2*) READ_ONE_BLOCK(inode_table.i_block[j]);
                int size = 0;
                int inode_size = inode_table.i_size;
                
                while (size < inode_size){
                    char file_name[255 + 1];
                    if (block_table->file_type == 2 || block_table->file_type == 1) {
                        strncpy(file_name, block_table->name, block_table->name_len);
                        file_name[block_table->name_len] = 0;
                    }else{
                        break;
                    }
                    size += block_table->rec_len;
                    if (strcmp(matchName, file_name) == 0) {
                            newPair.child_blocks = *block_table;
                            newPair.parent = inode_table;
                            newPair.inode_number = i;
                            return newPair;
                        }
                        block_table = (void *) block_table + block_table->rec_len;
                    }
            }
        }
    }
    return newPair;
}



//helper function to track the absolute path

//find next "/"
int next_path(char path[]){
    int len = strlen(path);
    if (len ==1){
        return 0;
    }
    int i;
    for (i = 1;i < len; i++){
        if (path[i] ==  '/' ){
            break;
        }
    }
    return i;
}

//get the directory name and find the inode contains it
int get_path_inode(char path[],struct ext2_inode* inode_table){
     char new_path[strlen(path)+1];
     memcpy(new_path, path, strlen(path));
     new_path[ strlen(path)] = 0;
     int pos;
     int inode_idx=0;
     
     while ((pos = next_path(new_path) ) > 1){        
        char* new = malloc(sizeof(char)*strlen(path));
        new = (char*)(new_path + pos);
        memcpy(new_path, new, strlen(new));
        new_path[strlen(new)] = 0;
        //call find_inode_index to find the inode        
        struct inode_dir_pair toFind  = fileExists(path);
        inode_idx = toFind.inode_number;
        if (inode_idx == -1){
            return -1; //directory not found
        }        
    }
    return inode_idx;
}

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

void print_inode_table(struct ext2_inode* inode_table){
    printf("type:%d, size:%d, link %d,blocks, %d \n",inode_table->i_mode,inode_table->i_size,inode_table->i_links_count,inode_table->i_blocks);
    int i;
    printf("block contents \n");
    for(i=0;i<inode_table->i_blocks;i++){
        printf("Block: %d \n",inode_table->i_block[i]);
    }
}

void print_dir_table(struct ext2_dir_entry_2* dir_table){
     char print_name[255+1];
     memcpy(print_name, dir_table->name, dir_table->name_len);
     print_name[dir_table->name_len] = 0;
     printf("[%d]File Name: %s, name length:%d, file length:%d , it is of type %d\n",dir_table->inode,print_name,dir_table->name_len,dir_table->rec_len,dir_table->file_type);            
}

void print_bitmap(){
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