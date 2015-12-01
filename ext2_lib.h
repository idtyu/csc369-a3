
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ext2.h"

int fd;
unsigned char* disk;
struct ext2_super_block *super_block;
struct ext2_group_desc *group_desc;
char* inode_bitmap;
char* block_bitmap;
struct ext2_inode* inode_table;
struct ext2_dir_entry_2*  block_table;

#define READ_ONE_BLOCK(blocknum) (disk+ (blocknum*EXT2_BLOCK_SIZE));
void init();
int bitmapToBlock(char* bitmap, unsigned int index);
void read_inode_table_n_blocks();
void getbitmap();
void printBits(unsigned char block);