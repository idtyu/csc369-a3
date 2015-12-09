ext2_ls:
takes 3 parameters, third parameter is absolute path or get an error message, the absolute path needs to start with "/", multiple "/" in path like "///home///User//" also works,  "/"  for root directory. 

ext2_rm:
takes 3 parameter, third parameter is absolute path or get an error message. The program used to track the path is same as ext2_ls. The program will check the path and ensure the target file exists in the path that is provided, or proper error message will be printed. It removes the file by adding its rec_len to previous rec_len, so the target file is skipped. Meanwhile, group descriptor, super_block and bitmap have also been modified according to the new structure. 

ext2_mkdir:
