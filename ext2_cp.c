#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "ext2_lib.h"


int allocateBlock(int begin) {
    int i;
    for(i=begin;i<super_block->s_blocks_count;i++){
        if(!bitmapToBlock(block_bitmap,i)){
            return i;
        }
    }
    /*No available space*/
    return -1;
}

char* setBitMap(char* bitmap,int position,int size){
    int i;
    unsigned char mask[] = {1, 2, 4, 8, 16, 32, 64, 128};
    for(i=0;i<(size/8);i++){      
        unsigned int arrayPosition = position / 8;
        unsigned int shiftPosition = position % 8 -1;
        bitmap[arrayPosition] |= mask[shiftPosition];
        return bitmap;
    }
    return NULL;
}
 
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ext2_cp <image file name> <source file> <Destination file>\n");
        exit(1);
    }
    init(argv[1]);
    inode_tables = (struct ext2_inode*) (disk + (group_desc->bg_inode_table * EXT2_BLOCK_SIZE));
    struct inode_dir_pair toFind = fileExists(argv[2]);
    if (toFind.inode_number >-1) {        
        int directory = get_path_inode(argv[3],inode_tables);
        struct ext2_inode newTable = inode_tables[directory];
        if(directory>-1){
            print_dir_table(&(toFind.child_blocks));
            /*copy diretory block to the new one*/
            unsigned int blockPosition = allocateBlock(inode_tables[directory].i_block[newTable.i_blocks-1]) + 1;           
            memcpy(disk+(blockPosition*EXT2_BLOCK_SIZE),&(toFind.child_blocks),EXT2_BLOCK_SIZE);
            /*modifiy current inode table*/
            int newBPosition = inode_tables[directory].i_blocks;
            inode_tables[directory].i_block[newBPosition] = blockPosition;
            inode_tables[directory].i_blocks++;
            int currentInodeBlock = group_desc->bg_inode_table + directory;
            printf("current block: %d \n",currentInodeBlock);
            memcpy(disk+(currentInodeBlock*EXT2_BLOCK_SIZE),&(newTable),EXT2_BLOCK_SIZE);
            char* newInode = READ_ONE_BLOCK(currentInodeBlock);
            printf("new inode table:");
            print_inode_table(newInode);
            /*set new bit maps*/
            char* newBitmap = setBitMap(block_bitmap,blockPosition,super_block->s_blocks_count);      
            memcpy(disk+(group_desc->bg_block_bitmap)*EXT2_BLOCK_SIZE,newBitmap,EXT2_BLOCK_SIZE);
            /*change group descriptor*/
            group_desc->bg_free_blocks_count--; 
            memcpy(disk+(group_desc->bg_inode_table+directory)*EXT2_BLOCK_SIZE,group_desc,EXT2_BLOCK_SIZE);
            if (msync(disk, 128 * 1024, MS_SYNC) == -1)
            {
                perror("Could not sync the file to disk");
            }
        }
    }
    return 1;
}