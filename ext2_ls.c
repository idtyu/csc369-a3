
#include "ext2_lib.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>



int main(int argc, char *argv[]) {
	if(argc != 3) {
        fprintf(stderr, "Usage: ext2_ls <image file name> <directory>\n");
        exit(1);
    }
    init(argv[1]);
	
    
    //get absolute path
    int path_len = strlen(argv[2]);
    char path_name[path_len+1];
    memcpy(path_name, argv[2], path_len);
    path_name[path_len] = 0;
    
    //check whether the path is valid
    if (path_name[0] != '/'){
		fprintf(stderr, "Please provide valid absolute path");
		exit(1);
	}
	
	
	get_path_inode(path_name, EXT2_ROOT_INO);
	
	//struct ext2_inode* inode = (struct ext2_inode*)(inode_table + 
	
	return 1;
	
    
    
}
