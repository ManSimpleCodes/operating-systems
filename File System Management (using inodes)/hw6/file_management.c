#include "file_system.h"

const int default_dir_inode_block_num = 0;

void create_sys_call(char file_name[])
{

	bool free_row = free_row_exists(default_dir_inode_block_num);
	if(! free_row)
		assign_new_data_block(default_dir_inode_block_num);

	inode this_inode = read_inode(default_dir_inode_block_num);

	//print(LOG, cat("FREE ROW? = ", itoa(this_inode.file_size)));
	print(LOG, cat("iNODE file size = ", itoa(this_inode.file_size)));
	print(LOG, cat("Next Block = ", itoa(this_inode.next_block)));

	bool success = write_dir_record(this_inode, file_name);
	if (success)
	{
		//update directory inode file size
		this_inode.file_size = this_inode.file_size + 32;
		write_inode(default_dir_inode_block_num, this_inode);
	}
}



void delete_sys_call(char file_name[])
{
	print(NOTIFY, "DELETE SYSTEM CALL!");
	//------- YOUR CODE HERE -----------
	inode dir_inode = read_inode(default_dir_inode_block_num); 
	int root_inode_num = get_file_name_inode(dir_inode, file_name);
	FILE *f =openDiskFile("r+");
	//Initialize byte offset, file for comparing, and total blocks of directory 
	int byte_offset = 0;
	char file[9];
	int total_blocks = dir_inode.blocks_assigned;
	//Variables used for for loops
	int i; 
	int k; 
	int block;
	//loops until it gets to the last block for directory
	for(block = 0; block < total_blocks; block++) {
		//If 8 blocks are read it will get to the next 8 blocks
		if (block % 8==0 && block!=0) {
			dir_inode=read_inode(dir_inode.next_block);
		}
			//Grabs the byte offset for block
			byte_offset	getBlockByteOffset(dir_inode.block[block % 8]) + 32;
			//Loops 15 times to check each line for file match
			for (k=0; k < 15; k++) {
				fseek(f, byte_offset + (32*k), SEEK_SET);
				//Gets each character one by one assuming a file size is 8 bytes:
				for (i=0; i < 8; i++) {
					file[i]=fgetc(f);
				}
				//If equal it will go back to beginning of that lines and will overwrite it(delete) 
				if (strcmp(file_name, file)==0) {
					fseek(f, byte_offset + (32*k), SEEK_SET);
					fputs("                               \n", f);
				}
			}
	}
	//Grabs file anode and total blocks assigned and frees the Initial block 
	inode file_inode=read_inode(root_inode_num);
	int blocks_assigned=file_inode.blocks_assigned; 
	free_block(file_inode.block[0] - 1);
	//loops until total blocks is reached 
	for(i=0; i < blocks_assigned; i++)
	{
		//This will grab the root block and moving to the next !node and delete the initial block of that anode 
		if ((i % 8) == 0 && i!=0) {
			int root_block = file_inode.next_block;
			file_inode = read_inode(file_inode.next_block);
			free_block(root_block);
		}
		free_block(file_inode.block[i % 8]);
	}
}




	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	
}


void read_sys_call(char file_name[])
{

	print(NOTIFY, "READ SYSTEM CALL!");

	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	inode dir_inode=read_inode(default_dir_inode_block_num);
	int file_inode_block_num = get_file_name_inode(dir_inode, file_name);
	inode file_inode = read_inode(file_inode_block_num);
	char *data = ""; 
	int block_idx;
	int blocks_assigned = file_inode.blocks_assigned;
	int num_inodes = (blocks_assigned / 8) + 1;
	print(LOG, cat("blocks_assigned: ", itoa(blocks_assigned)));
	int i;
	for (i=0;i<num_inodes;i++)
	{
		int blocks_left = blocks_assigned - (i*8);
		if (blocks_left > 8)
			blocks_left = 8;
		for (block_idx=0;block_idx<blocks_left;block_idx++)
		{
			char *block_data = read_data_block( file_inode.block[block_idx] );
			data = cat(data, block_data);
		}
		file_inode = read_inode(file_inode.next_block); //come back to this...
	}
	print(NOTIFY, cat("READ: ", data));
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------
	//------- YOUR CODE HERE -----------

}




void copy_sys_call(char src_name[], char dest_name[])
{

	print(NOTIFY, "COPY SYSTEM CALL!");
	inode dir_inode = read_inode(default_dir_inode_block_num);
	int root_inode_num = get_file_name_inode(dir_inode, dest_name);
	int current_inode_num = root_inode_num;

	int fd = open(cat("files/",src_name) , O_RDONLY);
	if (fd == -1)
	    print(ERROR, "ERROR COPY SRC FILE!\n");

	int bytes;
	do
	{
		char data[465];
		bytes = read(fd, data, 465);
		if (bytes > 0)
		{

			inode root_inode = read_inode(root_inode_num);

			print(LOG, cat("BLOCKS ASSIGNED: ",itoa(root_inode.blocks_assigned)));
			if(root_inode.blocks_assigned % 8 == 0 && root_inode.blocks_assigned != 0)
			{
				int inode_num = find_free_block();
				init_new_inode(inode_num);
				inode prev_inode = read_inode(current_inode_num);
				prev_inode.next_block = inode_num;
				write_inode(current_inode_num, prev_inode);
				if(root_inode_num == current_inode_num)
					root_inode.next_block = inode_num;
				current_inode_num = inode_num;
			}

			int new_data_block = assign_new_data_block(current_inode_num);
			write_data_block(new_data_block, data, bytes);

			//update root inode....
			root_inode.file_size = 	root_inode.file_size + bytes;
			root_inode.blocks_assigned++;
			write_inode(root_inode_num, root_inode);
		}
		else if (bytes == -1)
			print(ERROR, "ERROR - reading from file!");

	} while(bytes > 0);

	close(fd);
}
