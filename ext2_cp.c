#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ext2_lib.h"

struct inode_dir_pair fileExists(char* fileName){
    int inode_num = super_block -> s_inodes_count;
    int i;
    struct inode_dir_pair newPair;
    for(i=0;i<inode_num;i++){
        if(i==1){
            struct ext2_inode inode_table = inode_tables[i];
            int j;
            for(j=0;j<inode_table.i_blocks;j++){
                struct ext2_dir_entry_2* block_table =(struct ext2_dir_entry_2*) &inode_table.i_block[j];
                char print_name[256];
                memcpy(print_name, block_table->name, block_table->name_len);
                print_name[block_table->name_len] = '\0';
                printf("[%d]file name: %s \n",i,print_name);
                if(strcmp(print_name,fileName)==0){
                    printf("found!\n");
                    newPair.child_blocks = *block_table;
                    newPair.parent = inode_table;
                    return newPair;
                }
            }
        }else if(bitmapToBlock(inode_bitmap,i) && i >= 11 ){
            struct ext2_inode inode_table = inode_tables[i];
            int j;
            for(j=0;j<inode_table.i_blocks;j++){
                struct ext2_dir_entry_2*  block_table =  (struct ext2_dir_entry_2*)(& inode_table.i_block[j]);
                char print_name[256];
                memcpy(print_name, block_table->name, block_table->name_len);
                 print_name[block_table->name_len] = '\0';
                printf("[%d]file name: %s \n",i,print_name);
                if(strcmp(print_name,fileName)==0){
                    printf("found!\n");
                    newPair.child_blocks = *block_table;
                    newPair.parent = inode_table;
                    return newPair;
                }
            }
        }
    }
    return newPair;
}


int main(int argc, char *argv[]) {
	if(argc != 4) {
            fprintf(stderr, "Usage: ext2_cp <image file name> <source file> <Destination file>\n");
            exit(1);
        }	
        init(argv[1]);
        inode_tables = (struct ext2_inode*)(disk + (group_desc->bg_inode_table * EXT2_BLOCK_SIZE));         
        struct inode_dir_pair toFind = fileExists(argv[2]);
        if(toFind.parent.i_block!=NULL){
            printf("File: %s, toFind:%s \n",argv[2],toFind.child_blocks.name);
        }
        return 1;
}