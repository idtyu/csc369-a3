
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ext2.h"

int fd;
unsigned char* disk;
struct ext2_super_block *super_block;
struct ext2_group_desc *group_desc;
char* inode_bitmap;
char* block_bitmap;
struct ext2_inode* inode_tables;
struct ext2_dir_entry_2*  block_tables;
int block_count;
int free_inode;
int free_block;
struct inode_dir_pair newPair;

struct inode_dir_pair{
    struct ext2_inode parent;
    struct ext2_dir_entry_2 child_blocks;
    int inode_number;
};


#define READ_ONE_BLOCK(blocknum) (disk+ (blocknum*EXT2_BLOCK_SIZE));
void init();
int bitmapToBlock(char* bitmap, unsigned int index);
void read_inode_table_n_blocks();
void getbitmap();
void printBits(unsigned char block);
int next_path(char path[]);
int get_path_inode(char path[],struct ext2_inode* inode_table);
void print_inode_table(struct ext2_inode* inode_table);
void print_dir_table(struct ext2_dir_entry_2* dir_table);
void print_inode_table(struct ext2_inode* inode_table);
struct inode_dir_pair fileExists(char* matchName);