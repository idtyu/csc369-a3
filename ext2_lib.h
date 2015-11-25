#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ext2.h>

int fd;
unsigned char *disk;
struct ext2_super_block *super_block;
struct ext2_group_desc *group_desc[];