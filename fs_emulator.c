#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_INODES 1024 //max number of inodes
#define NAME_LEN 32 //max # of characters for file/directory names

//----------------------------------------------------------------------------------------
//INODE STRUCT
typedef struct{
	uint32_t inode;
	uint32_t parent_inode;
	char type; //directory 'd' or file 'f'
	char name[NAME_LEN]; //name of file/directory
} Inode;

//GLOBAL VARIABLES
Inode inode_list[MAX_INODES];//array of inodes, each inode represents file or directory
size_t inode_count = 0;//count of # of inodes in list, initialized to zero
uint32_t curr_inode = 0; //var to keep track of curr inode

//----------------------------------------------------------------------------------------
//HELPER FUNCTIONS
//helper to check if user commands have all expected compoents
bool valid(char *cmd, char *arg, char *extra){
        //takes_none: commads that don't expect any arguments
        //takes_arg: comands that expect one argument
        bool takes_none = (strcmp(cmd, "exit") == 0 || strcmp(cmd, "ls") == 0);
        bool takes_arg = (strcmp(cmd, "cd") == 0 || strcmp(cmd, "mkdir") == 0 || strcmp(cmd, "touch") == 0);

        if(!takes_none && !takes_arg){ //neither of the recognized categories, so unknown command
                printf("ERROR: unrecognized command\n");
                return false;
        }
        if(takes_none && arg != NULL){ //exit and ls are inputted w arguments
                printf("ERROR: %s does not expect arguments\n", cmd);
                return false;
        }
        if(takes_arg && (arg == NULL || extra != NULL)){ //cd, mkdir, touch w no arg or extra args
                printf("ERROR: %s expects exactly one argument\n", cmd);
                return false;
        }
        return true;
        //true = commands and arguments match the expected amount
        //false = commands and arguments don't match the expected amount

}

//convert uint32 to string (used in mkdir when naming the new inode)
char *uint32_to_str(uint32_t i){
	int length = snprintf(NULL, 0, "%u", i);
	if(length < 0) return NULL;
	
	char *str = (char*) malloc(sizeof(char) * (length + 1));
	if (str == NULL) return NULL;

	snprintf(str, length + 1; "%u", i);
	return str;
}
//returns possible inode number for new entry, one past highest inode number in use)
uint32_t find_free_inode(){
	uint32_t highest = 0;
	for(size_t i = 0; i < inode_count; i++){
		if(inode_list[i].inode > highest){
			highest = inode_list[i].inode;
		}
	}
	if(highest + 1 >= MAX_INODES){
		return -1; //no inodes availaible in range 0 to 1023
	}
	return highest + 1;
}

//returns file pointer to file with same name as inode_number for wirting, used in mkdir and touch
FILE *create_inode_file(uint32_t inode_number, const char *mode){
	char *filename = uint32_to_str(inode_number); // uint32 inode number to string
	if(filename == NULL){ //check if successful
		return NULL;
	}
	FILE *fp = fopen(filename, mode); //open for writing
	free(filename);
	return fp;
}

//----------------------------------------------------------------------------------------
//REQUIRED FUNCTIONS
//(REQUIREMENT 2)
void load_inode_list(const char *path){
	//read inode_list from file path to inode_list array
	//update inode_count
	//inode_list contains one of each unique inode numbers

	FILE *fp = fopen(path, "rb");//open specified file to read
	if(fp == NULL){ //check successfull open
		printf("ERROR: unable to open file %s for reading\n", path);
		exit(1);
	}
	else{//read from file
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

		fclose(fp); //close file pointer after reading

 		//emulation starts in the directory at inode 0, if this isn't a directory
		//then report error and terminate
		if(inode_count == 0 || inode_list[0].inode != 0 || inode_list[0].type != 'd'){
			printf("ERROR: inode 0 is not a directory or is not in use\n");
			exit(1);	
		}		

		//curr_inode (current working directory) is already initialized to 0
	
		//fclose(fp);//close file pointer after reading
	}
}
//(REQUIREMENT 4)
void save_inode_list(const char *path){ //after exit
	//write content of inode_list to file path
	FILE *fp = fopen(path, "wb");
	if(fp == NULL){ //check successfull open
                printf("ERROR: unable to open file %s for writing\n", path);
                exit(1);
        }
        else{
		for(size_t i = 0; i < inode_count; i++){
			//CHECK FORMATTING FOR READING AND WRITING WHILE TESTING!!!
			if(fwrite(&inode_list[i], sizeof(Inode), 1, fp) != 1){
				//nothing was written:
				printf("ERROR: failed to write inode %u\n", inode_list[i].inode);
				fclose(fp);//close before exiting on error
				exit(1);
			}
		}
		fclose(fp);
	}
}

void change_dir(const char *name){ //cd
	//iterate through inode_list to search whether a directory with name exists
	//if yes, then set curr_inode value to the inode at index i
	//otherwise print an error message to indicate that specified name isn't a directory (DNE or regular file), 
	//report error and don't change directory
	
	for(size_t i = 0; i < inode_count; i++){
		//check if parent and current directories match, name matches and is a directory, if yes then switch
		if(inode_list[i].parent_inode == curr_inode 
				&& strncmp(inode_list[i].name, name, NAME_LEN) == 0 
				&& inode_list[i].type == 'd'){
	
			curr_inode = inode_list[i].inode; //directory with specified name exists, so change directory
			return; //no need to keep iterating since file and directory names are unique
		}	
	}
	printf("ERROR: specified name is not a directory or does not exist\n");

}


void list_contents(){ //ls
	//iterate through inodes_list and print members of those elements in the list for which 
	//the parent_inode is the curr_inode
	
	for(size_t i = 0; i < inode_count; i++){//iterate through inodes_list
		if(inode_list[i].parent_inode == curr_inode){//check parent and curr_node then print in specified format
			printf("inode: %u, type: %c, name: %.32s\n", inode_list[i].inode, inode_list[i].type, inode_list[i].name);
			//ex: inode: 0, type: d, name: demo
		}
	}
	
}

void create_dir(const char *name){ //mkdir
	//iterate through inode_list to see if the name of the directory exists, if yes then exit
	for(size_t i = 0; i < inode_count; i++){
		//check parent directory then comapre names
		if(inode_list[i].parent_inode == curr_inode && strncmp(inode_list[i].name, name, NAME_LEN) == 0){
			printf("ERROR: entry with name %s already exists in this directory\n", name);
			return;
		}
	}

	uint32_t free_inode = find_free_inode();//unused inode number
	if(free_inode == UINT32_MAX){//UINT32_MAX is the equivalent of checking for -1 in int
		printf("ERROR: no availaible inodes\n");
		return;
	}

	//check to see if inode_count is 1024, if yes then exit
	if(inode_count == MAX_INODES){
		printf("ERROR: max inodes reached\n");
		return;
	}
	
	//create new inode with new inode)count, set its parent_inode and type and also its name
	//new inode should be created at the lowest availaible inode, which is one after the inode_count?
	Inode new_inode;
	new_inode.inode = free_inode;//inode number
	new_inode.parent_inode = curr_inode;//parent inode is the same as the directory mkdir is called from
	new_inode.type = 'd';//creating directory
	strncpy(new_inode.name, name, NAME_LEN); //name is the name of the created directory
	

	//create file with its name as the inode number
	FILE *fp = create_inode_file(new_inode.inode, "wb");
	if(fp == NULL){ //file creation failed
		printf("ERROR: couldn't create file for inode %u\n", new_inode.inode);
		return;
	}
	//write . and .. values into this new file on two different lines
	char line[64];
	int len = snprintf(line, sizeof(line), ". %u\n", new_inode.inode);
	fwrite(line, 1, len, fp); //write len bytes to new file w/o \0
	len = snprintf(line, sizeof(line), ".. %u\n", curr_inode); //curr_inode is the parent
	fwrite(line, 1, len, fp);
	fclose(fp);
	
	//add inode to current directory's file
	FILE *fp = create_inode_file(curr_inode, "ab"); //from slides: "ab" appends instead of overwriting
	//curr_inode is the current directory, so we update that with new inode number and name
	if(fp == NULL){
		printf("ERROR: couldn't update current directory with new inode\n");
		return;
	}
	fwrite(&new_inode.inode, sizeof(uint32_t), 1, fp); //inode number
	fwrite(new_inode.name, NAME_LEN, 1, fp); //32 char (max) name

	fclose(fp);

	//add new_inode to inode_list and increment inode_count
	inode_list[inode_count] = new_inode;
	inode_count++;
}


void create_file(const char *name){ //touch
	//iterate through inode_list to see if filename exists, if yes then exit
	//check is the inode_count is 1024, if yes then exit
	//create new entry for an inode in inode_list
	//create file with its name as the inode number
	//write into this file its filename
}

//----------------------------------------------------------------------------------------
//MAIN
int main(int argc, char *argv[]){
	//(REQUIREMENT 1) accept directory name as command line argument
	if(argc == 2){
		//(REQUIREMENT 2)
		//switch program's working directory into emulated folder fs (using chdir)
		if(chdir(argv[1]) == 0){
			//printf("Directory changed to %s\n", argv[1]);
			//path argument is inodes_list since we're already inside fs directory
			load_inode_list("inodes_list");	
		
			//(REQUIREMENT 3)
			char *input = NULL;
			size_t cap = 0;

			while(1){//loop until user types ctrl+d or exit
				printf("> ");
				//take user commands as input from stdin
				if(getline(&input, &cap, stdin) == -1) break;
				
				//parse user input
				char *cmd = strtok(input, " \t\n");//can be separated by space, tab, or \n
				char *arg = strtok(NULL, " \t\n"); //null if there is no second word
				char *extra = strtok(NULL, " \t\n");//more than one argument provided

				//check if empty command
				if(cmd == NULL) continue;
				//check validity
				if(!valid(cmd, arg, extra)) continue;

				//32 filename length limit
				char arg_len[NAME_LEN + 1] = "";
				if(arg != NULL){
					strncpy(arg_len, arg, NAME_LEN); //copy 32 characters
					arg_len[NAME_LEN] = '\0'; //33rd character is null terminator
				}
				//arg_len is the argument modified to fit the limit of 32 chars for directory and file names, 
				//so all functions use arg_len as parameters since it's the shortened version of arg to meet limits 


				//commands and their behavior
				if(strcmp(cmd, "exit") == 0){
					//save_inode_list (REQUIREMENT 4)
					break;//break out of while loop
				}
				else if(strcmp(cmd, "cd") == 0){ 
					change_dir(arg_len);
				}
				else if(strcmp(cmd, "ls") == 0){
					list_contents();
				}
				else if(strcmp(cmd, "mkdir") == 0){
					create_dir(arg_len);
				}
				else if(strcmp(cmd, "touch") == 0){
					create_file(arg_len);
				}
			}//end while(1)
		
			free(input); //free stdin input read using getline
			save_inode_list("inodes_list");
			return 0;
		}
		else{//coudn't change into file sys directory
			perror("chdir failed\n");
			exit(1);
		}
	}
	else{
		printf("ERROR: zero or more than one command line arguments were provided\n");
		exit(1);
	}
	return 0;
}


