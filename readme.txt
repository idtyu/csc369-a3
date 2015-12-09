ext2_ls:
takes 2 parameters, the first is the name of an ext2 formatted virtual disk, the second parameter is absolute path or get an error message, the absolute path needs to start with "/", multiple "/" in path like "///home///User//" also works,  "/"  for root directory. 

ext2_rm:
takes 2 parameters, the first is the name of an ext2 formatted virtual disk, the second parameter is absolute path or get an error message.  The program used to track the path is same as ext2_ls. The program will check the path and ensure the target file exists in the path that is provided, or proper error message will be printed. It removes the file by adding its rec_len to previous rec_len, so the target file is skipped. Meanwhile, group descriptor, super_block and bitmap have also been modified according to the new structure. 

ext2_mkdir:
takes 2 parameters, the first is the name of an ext2 formatted virtual disk, the second parameter is absolute path or get an error message. To make a new dir,we need at least one new inode and one new dir, store the dir number in the new inode.i_block[0], then create suitable structure in the new block. When links the new inode to parent dir's block, previous rec_len is checked in order to have enough room to store the new dir's name. If not, find another empty block and links it to parent dir's inode.

ext2_cp:


