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
    unsigned char mask[] = {1, 2, 4, 8, 16, 32, 64, 128};
    unsigned int arrayPosition = position / 8;
    unsigned int shiftPosition = (position % 8) -1;
    unsigned char newBit = bitmap[arrayPosition];
    newBit |= mask[shiftPosition];
    memcpy(&bitmap[arrayPosition],&newBit,8);
    return bitmap;
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
        struct ext2_inode newTable = *(struct ext2_inode*)READ_ONE_BLOCK(directory);
        if(directory>-1){
            /*copy diretory block to the new one*/
            unsigned int blockPosition = allocateBlock(newTable.i_block[newTable.i_blocks-1]) + 1;           
            memcpy(disk+(blockPosition*EXT2_BLOCK_SIZE),&(toFind.child_blocks),EXT2_BLOCK_SIZE);
            /*modifiy current inode table*/
            int newBPosition = newTable.i_blocks;
            memcpy(newTable.i_block+newBPosition,&blockPosition,sizeof(unsigned int));
            unsigned int temp_block = newTable.i_blocks++;
            memcpy(&newTable.i_blocks,&temp_block,sizeof(unsigned int));
            newTable.i_blocks++;
            memcpy(disk+((group_desc->bg_inode_table+directory)*EXT2_BLOCK_SIZE),&(newTable),EXT2_BLOCK_SIZE);
            
            /*set new bit maps*/
            //setBitMap(block_bitmap,blockPosition,super_block->s_blocks_count);   
                      
            printf("New Structure: \n");
            //print_bitmap();
            print_inode_table(&inode_tables[directory]);
            
            int k = msync(disk, 128 * 1024, MS_SYNC) ;
            printf("%d \n",k);
            if (k == -1)
            {
                perror("Could not sync the file to disk");
            }
        }
    }
    return 1;
}