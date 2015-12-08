#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"
#include <string.h>
#include <errno.h>

unsigned char *disk;
char dir[1024+1];
void* inodebitmap;
char* blockbitmap;
struct ext2_inode* inode_table;

//make them global

int find_inode_index(char* dir, int i,struct ext2_inode* inode_table,int flag);
void bitmap_reset(void* bitmap, int index, int value);
void change_bitmap( struct ext2_inode result_inode, char* blockbitmap,char* inodebitmap,int inode_num);
int count_free(char* bitmap,int total_num);

#define MAX_PATH_DIR 100 /*the absolute path max have 100 die*/

#define BASE_OFFSET 1024  /* location of the super-block in the first group */

#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*1024)


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
int get_path_inode(char path[],int inode_idx,struct ext2_inode* inode_table){
     char new_path[strlen(path)+1];
     memcpy(new_path, path, strlen(path));
     new_path[ strlen(path)] = 0;
     int pos;
     
     
     while ((pos = next_path(new_path) ) > 1){
        
        int i;
        for (i = 0; i < pos - 1; i++){
            dir[i] = new_path[i+1];
        }
        dir[pos-1] = 0;

        
        int size = 0;
        
        char* new = malloc(sizeof(char)*strlen(path));
        new = (char*)(new_path + pos);
        memcpy(new_path, new, strlen(new));
        new_path[strlen(new)] = 0;
        if (strlen(new_path) < 2){
            inode_idx = find_inode_index(dir, inode_idx,inode_table,1); // 1 for find a file
        }else{
        //printf("%s\n",new_path);
        //printf("%s\n",dir);
        //call find_inode_index to find the inode
        inode_idx = find_inode_index(dir, inode_idx,inode_table,2); // 2 for find a directory
        }
        //printf("find:%d\n",inode_idx);
        if (inode_idx == -1){
            return -1; //directory not found
        }

    }
    return inode_idx;
}
int find_inode_index(char* dir, int inode_num, struct ext2_inode* inode_table, int flag){
    int found = 0;
    struct ext2_inode target_inode = inode_table[inode_num-1];
    int num_block = target_inode.i_size / EXT2_BLOCK_SIZE;
    struct ext2_dir_entry_2* entry;
    int block_num;
    int idx;
    int return_value = -1;
    for (idx=0; idx< num_block; idx++){
                block_num = target_inode.i_block[idx];
                entry = (struct ext2_dir_entry_2*)(disk + EXT2_BLOCK_SIZE * block_num); 

                int size = 0;
                
                int last_rec_len = 0;
                int curr_pos = 0;
                
                while (size < EXT2_BLOCK_SIZE){
                    size += entry->rec_len;
                       //file type
                        //file_type   Description
                        //0   Unknown
                        //1   Regular File
                        //2   Directory
                        //3   Character Device
                        //4   Block Device
                        //5   Named pipe
                        //6   Socket
                        //7   Symbolic Link

                        if(entry->file_type == 1 || entry->file_type == 2){
                        char file_name[255+1];
                        memcpy(file_name, entry->name, entry->name_len);
                        file_name[entry->name_len] = 0;
                        
                            if (strncmp(file_name, dir, entry->name_len) == 0 && strlen(dir) == entry->name_len){
                                if (entry->file_type != flag) return -2;
                                else if (flag != 1) return entry->inode;
                                else{
                                    found = 1;
                                    break;
                                }
                                //return entry->inode;
                            }
                        }

                        
                        
                        
                        if (found == 0){
                            curr_pos += entry->rec_len;
                            last_rec_len = entry->rec_len;
                            //printf("last len: %d\n",last_rec_len);
                        }
                        entry = (void *)entry + entry->rec_len;

                        
               
                }
            
        
        if (found == 0) return -1; //return -1 for not found
        //handle the delete movement
        //return entry->inode;
        return_value = entry->inode;
        //printf("%d",return_value);
        
        
            
            
        int r_len = entry->rec_len;
        entry = (void*)entry - last_rec_len;//return to last position
        entry->rec_len = r_len+last_rec_len; //junmp the target file length
        //printf("len: %d\n",entry->rec_len);
        //printf("about to return\n");
        bitmap_reset(inodebitmap, return_value - 1, 0);
        //clean_inode(int return_value);
        return return_value;
        //printf("last len: %d\n",last_rec_len);

           

       
       
    }
    //return entry->inode;
    return return_value;
} 

//void clean_inode(int inode_idx){
    //struct ext2_inode target_inode = inode_table[inode_idx-1];
    
//}

void bitmap_reset(void* bitmap, int index, int value){
    unsigned int arrayPosition = index / 8;
    unsigned int shiftPosition = index % 8;
    char* byte = bitmap + arrayPosition;
    
    *byte = (*byte & ~(1 << shiftPosition)) | (value << shiftPosition);
    //return 1;



}

int count_free(char* bitmap,int total_num){
    int i;
    int count = 0;
    for (i = 0; i < total_num/8; ++i)
    {
        int bit;
        for ( bit= 0; bit < 8; bit++){
        count += (bitmap[i] >> bit)& 1;
        }
    }
    return (total_num - count);


}
   






int main(int argc, char **argv) {

    if(argc != 3) {
        fprintf(stderr, "Usage: ext2_ls <image file name> <directory>\n");
        exit(1);
    }
    
    
    
    //get absolute path
    int path_len = strlen(argv[2]);
    char path_name[path_len+1];
    memcpy(path_name, argv[2], path_len);
    path_name[path_len] = 0;
    
    //check whether the path is valid
    if (path_name[0] != '/'){
        fprintf(stderr, "Please provide valid absolute path\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
	perror("mmap");
	exit(1);
    }
    
    //get the suprt block and group descripter
    struct ext2_super_block *sb = (struct ext2_super_block *)(disk + 1024);
    struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + 1024*2);

    
    blockbitmap = (char*)(disk + (gd->bg_block_bitmap)* EXT2_BLOCK_SIZE);
    inodebitmap = disk + (gd->bg_inode_bitmap)* EXT2_BLOCK_SIZE;
    inode_table = (struct ext2_inode*)(disk + (gd->bg_inode_table * EXT2_BLOCK_SIZE));

    int total_inode = sb->s_inodes_count;
    int total_block = sb->s_blocks_count;
  
    //char* path_list[255+1];

    int inode_num = get_path_inode(path_name, EXT2_ROOT_INO, inode_table);
    //printf("%d\n",inode_num);
    if (inode_num == -2){
        //printf("rm: %s:  is a directory\n",dir);
        errno = EISDIR;
        perror(dir);
        exit(errno);
        return -1;
    }else if (inode_num == -1){
        //printf("rm: %s: No such file or directory\n",dir);
        errno = ENOENT;
        perror(dir);
        exit(errno);

    }
    //TODO
    struct ext2_inode result_inode = inode_table[inode_num - 1];
    result_inode.i_links_count -= 1;
    int num_block = result_inode.i_size / EXT2_BLOCK_SIZE;
    int idx;
    for (idx=0; idx< num_block; idx++){
        int block_num = result_inode.i_block[idx];
        bitmap_reset(blockbitmap,block_num,0);

    }
    




    //ls_print(inode_table,inode_num-1);
   //delete_inode(inode_table,inode_num -1)
    gd->bg_free_blocks_count = count_free(blockbitmap,total_block);
    //printf("done\n");
    gd->bg_free_inodes_count = count_free(inodebitmap,total_inode);

   return 0;

}
