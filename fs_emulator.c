#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define MAX_INODES 1024
#define NAME_LEN 32

//use this structure to read/write from the binary file
typedef struct{
	uint32_t inode;
	uint32_t parent_inode;
	char type;
	char name[32];
} Inode;

//recomended variables to use
int main(void){
	Inode inode_list[MAX_INODES];//array of inodes, each inode represents file or directory
	size_t inode_count = 0;//count of # of inodes in list, initialized to zero
	uint32_t curr_inode = 0; //var to keep track of curr inode

	return 0;
}

//function headers from slides
void load_inode_list(const char *path){
	//read inode_list from file path to inode_list array
	//update inode_count
	//reminderL directory will have only one inode_list with a given number of inodes in the list
}

void save_inode_list(const char *path){
	//write content of inode_list to file path

}

void change_dir(const char *name){
	//iterate through inode_list to search whether a directory with name alr exists
	//if yes, then set curr_inode value to the index of the list with that name
	//otherwise print an error message to indicate that directory was not found
}

void list_contents(){
	//iterate through inodes_list and print members of those elements in the list for which the parent_inode is the curr_inode
}

void create_dir(const char *name){
	//iterate through inode_list to see if the name of the directory exists, if yes then exit
	//check to see if inode_count is 1024, if yes then exit
	//create new inode with new inode)count, set its parent_inode and type and also its name
	//create file with its name as the inode number
	//white into this file its . and .. values
}

void create_file(const char *name){
	//iterate through inode_list to see if filename exists, if yes then exit
	//check is the inode_count is 1024, if yes then exit
	//create new entry for an inode in inode_list
	//create file with its name as the inode number
	//write into this file its filename
}





