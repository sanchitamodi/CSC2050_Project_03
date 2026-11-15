#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define MAX_INODES 1024
#define NAME_LEN 32

//----------------------------------------------------------------------------------------
//Inode structure
typedef struct{
	uint32_t inode;
	uint32_t parent_inode;
	char name[NAME_LEN]; //name of file/directory
	char type; //directory 'd' or file 'f'
} Inode;

//GLOBAL VARIABLES
Inode inode_list[MAX_INODES];//array of inodes, each inode represents file or directory
size_t inode_count = 0;//count of # of inodes in list, initialized to zero
uint32_t curr_inode = 0; //var to keep track of curr inode
//----------------------------------------------------------------------------------------
//FUNCTIONS
//(REQUIREMENT 2)
void load_inode_list(const char *path){
	//read inode_list from file path to inode_list array
	//update inode_count
	//inode_list contains one of each unique inode numbers

	FILE *fp = fopen(path, "rb");//open specified file to read
	if(fp == NULL){ //check successfull open
		printf("ERROR: unable to open file %s\n", path);
		exit(1);
	}
	else{//read till EOF or inode_count = MAX_INODES
		Inode temp; //to read and validate indicators before storing to inode_list
		//read from file and store to temp for validation:
		while(fread(&temp, sizeof(Inode), 1, fp) == 1){
			//validate that indicators are valid:
			if(temp.inode >= MAX_INODES||((temp.type != 'd')&&(temp.type != 'f'))){
				printf("INVALID INODE, ignoring inode %u\n", temp.inode);
				continue;
			}	

			//any inode number outside the range from 0 to MAX_INODES are ignored:
			if(inode_count >= MAX_INODES){
				printf("WARNING: Reached maximum inode limit\n");
				break;
			}

			//passed validations so add temp to inodes_list
			inode_list[inode_count] = temp;
			inode_count++;
		}
 		//emulation starts in the directory at inode 0, if this isn't a directory
		//then report error and terminate
		if(inode_count == 0 || inode_list[0].inode != 0 || inode_list[0].type != 'd'){
			printf("ERROR: inode 0 is not a directory or not is not in use\n");
			exit(1);	
		}		

		//curr_inode (current working directory) is already initialized to 0
	
		fclose(fp);//close file pointer after reading
	}
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


int main(int argc, char *argv[]){
	//(REQUIREMENT 1) accept directory name as command line argument
	if(argc == 2){
		//(REQUIREMENT 2)
		//switch program's working directory into emulated folder fs (using chdir)
		if(chdir(argv[1]) == 0){
			printf("Directory changed to %s", argv[1]);
			//path argument is inodes_list since were already inside fs
		
		}
		else{
			perror("chdir failed");
		}

	}
	else{
		printf("ERROR: zero or more than one command line arguments were provided.");
	}



	return 0;
}


