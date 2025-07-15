/*************************************************************************************
 * shellfuncts.c - code definitions for your functions
 *
 *************************************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include "shellfuncts.h"

/*************************************************************************************
 * hello - sends hello world to the user! I'm doing proper function commenting so future
 *         coders might not find my code as painful.
 *
 *		Params:	param1 - I explain my parameters, like this is 1 for American, 2 for
 *                      Australian
 *
 *		Returns: always returns 1, no matter what. Fairly useless.
 *
 *************************************************************************************/

typedef enum{
	CMD_CREATE = 2,
	CMD_UPDATE = 4,
	CMD_LIST = 2,
	CMD_DIR = 1,
	CMD_HALT = 1
} MaxParams;

const char* BASE_DIR = "./testing_folder";
const int MAX_PATH_LENGTH = 100;

int hello(int param1) {
	// I'm commenting to explain that this checks param and changes the message
	if (param1 == 1)
		send_msg("Hello world!\n");
	else
		send_msg("G'day world!\n");

	// Return 1 because, why not.
	return 1;
}

/*************************************************************************************
 * hello - sends hello world to the user! I'm doing proper function commenting so future
 *         coders might not find my code as painful.
 *
 *    Params:  param2 - not a very good parameter name - something better might be
 *                      say, msgstr or sendtext
 *
 *************************************************************************************/

void send_msg(const char *param2) {
	printf("%s", param2);
}

/*************************************************************************************
 * parse_command - Reads the command line, and returns a pointer to the tokenized parameters.
 * Params: command - The actual input from the user
 *         expected_wc - the amount of parameters there SHOULD be.
 *
 * Returns: the tokenized array of the command.  So "hello world" becomes 'hello'[0] and 'world'[1]
 *************************************************************************************/

char** parse_command(const char* command) {
        //allocate memory for a copy, check if null just in case it fails
        char* command_copy = strdup(command);
        if (!command_copy) return NULL;

        //initialize the counter and find first token
        int i = 0;
        char* token = strtok(command_copy, " ");

	//make sure the parameter count provided is correct, given the command
	int expected_wc = get_command_params(token);
	if (expected_wc == 0) return NULL;

        //allocate memory for the return tokenized array, check if null just in case it fails
        char **words = malloc(expected_wc * sizeof(char*));
        if (!words) return NULL;

        //keep tokenizing untill no more words exist.
        while (token) {
		if (token[0] == '"') {
			// start of a quoted string
			size_t len = strlen(token);

			char* quoted = malloc(256);
			if (!quoted) return NULL;

			// remove the starting quote
			strcpy(quoted, token + 1);

			// if token ends with a quote, thats the end of the param
			if (quoted[strlen(quoted)-1] == '"') {
				// null terminate it
				quoted[strlen(quoted)-1] = '\0';
			} else { //if not, 
				// keep adding on the contents until the closing quote 
				while ((token = strtok(NULL, " ")) != NULL) {
					strcat(quoted, " ");
					strcat(quoted, token);
					len = strlen(quoted);
					if (quoted[len-1] == '"') {
						//null terminate it
						quoted[len-1] = '\0';  
						break;
					}
				}
			}

			words[i++] = strdup(quoted);
			free(quoted);
			token = strtok(NULL, " ");
		} else {
			words[i++] = strdup(token);
			token = strtok(NULL, " ");
		}
        }
	//ensure param count is the same
	if (i != expected_wc) {
		free(command_copy);
		return NULL;
	}

	// we do not need this temp copy anymore.
        free(command_copy);
        return words;
}

/*************************************************************************************
 * exe_with_fork - Fork the process before executing
 * Params: words - the actual tokenized command and parameters, for exe_command
 *
 * this just serves to clean up the main method.
 *************************************************************************************/
void exe_with_fork(const char** words){
	pid_t pid = fork();	

	if (pid == 0){ //child
		printf("[+][+][+] Child PID: %d\n\n", getpid());
		exe_command(words);
	} else if (pid > 0){ //parent
		printf("[+][+] Parent PID: %d Waiting for [Child PID: %d]\n", getpid(), pid);
		waitpid(pid, NULL, 0);
	} else {
		perror("> Fork failed");
	}
}

/*************************************************************************************
 * get_command_params - given the first word, it redirects
 * Params: first_word - this is main command being requested.
 *
 * Returns: the amount of parameters (main command included) the command should have
 *************************************************************************************/
int get_command_params(const char* first_word){
	if(strcmp(first_word, "create") == 0) {
		return CMD_CREATE;	
	} else if(strcmp(first_word, "update") == 0) {
		return CMD_UPDATE;
	} else if(strcmp(first_word, "list") == 0) {
		return CMD_LIST;
	} else if(strcmp(first_word, "dir") == 0) {
		return CMD_DIR;
	} else if(strcmp(first_word, "halt") == 0) {
		return CMD_HALT;
	} else {
		return 0;
	}
}


/*************************************************************************************
 * exe_command - given the first word, it redirects execution to the correct function, for that command.
 * Params: command - the command the user wants to execute: this is the entire line, not just the first word
 *
 * This function exists only to make the break up of logic easier to comprehend. 
 *************************************************************************************/
void exe_command(const char** command){
	const char* first_word = *(command + 0);
	if(strcmp(first_word, "create") == 0) {
		exe_create(command);
	} else if(strcmp(first_word, "update") == 0) {
		exe_update(command);
	} else if(strcmp(first_word, "list") == 0) {
		exe_list(command);
	} else if(strcmp(first_word, "dir") == 0) {
		exe_dir();
	}
}


/*************************************************************************************
 * exe_create - execute the "create" command, by making a file and closing it.
 * Params: command - the actual command, but all we really need at this point is the command[1], which is the filename.
 *
 * No return, only serves to create and new file safely.
 *************************************************************************************/
void exe_create(const char** command){
	//only one arg should exist, and it will be the filename
	const char* filename = *(command + 1);

	char full_path[MAX_PATH_LENGTH];
	// Create the full path safely, under the testing directory so that this is all contained.
	snprintf(full_path, sizeof(full_path), "%s/%s", BASE_DIR, filename);

	//read file first to see if it exists
	FILE* fp = fopen(full_path, "r");
	if (fp != NULL) {
		printf("> ERROR: File already exists.");
		exit(0);
	}
	
	// Open the file and have saftey check
	fp = fopen(full_path, "w");
	if (fp == NULL) {
		perror("> fopen failed");
		exit(0);
	}
	printf("> New file created: %s", full_path); 
	fclose(fp);
	
	exit(0);
}

/*************************************************************************************
 * exe_update - execute the "update" command, by creating/updating a file with repeated text, designated by user.
 * Params: command - the actual command with parameters. [1] is the filename, [2] is number, [3] is text.
 *
 * the "text" will be repeated a "number" amount of times, in file "filename".
 *************************************************************************************/
void exe_update(const char** command){
	// organize args
	const char* filename = *(command + 1);
	const int number = atoi(*(command + 2));
	const char* text = *(command + 3);

	// open file

	char full_path[MAX_PATH_LENGTH];
	// Create the full path safely, under the testing directory so that this is all contained.
	snprintf(full_path, sizeof(full_path), "%s/%s", BASE_DIR, filename);
	// Open the file and have saftey check
	FILE* fp = fopen(full_path, "a");
	if (fp == NULL) {
		perror("> File open error.");
		exit(0);
	}

	// loop through the file and write, while sleeping to emulate a slow system.
	
	for (int i = 0; i < number; i++){
		fprintf(fp, "%s\n", text); 
		fflush(fp);
		sleep(strlen(text)/5);
	}
	printf("> File updated: %s", full_path);
	fclose(fp);
	
	exit(0);
}

/*************************************************************************************
 * exe_list - execute the "list" command, which will esssentially "cat" the contents of the file.
 * Params: command - the actual command with parameters. [1] is the filename
 *************************************************************************************/
void exe_list(const char** command){
	//first arg will be filename
	const char* filename = *(command + 1);

	// oppen the file to read

	char full_path[MAX_PATH_LENGTH];
	// Create the full path safely, under the testing directory so that this is all contained.
	snprintf(full_path, sizeof(full_path), "%s/%s", BASE_DIR, filename);
	// Open the file and have saftey check
	FILE* fp = fopen(full_path, "r");
	if (fp == NULL) {
		perror("> File open error.");
		exit(0);
	}
	fclose(fp);

	//Just cat the file
	execl("/bin/cat", "cat", full_path, NULL);
	exit(0);
}

/*************************************************************************************
 * exe_dir - execute the "dir" command, by listing the testing_dir
 *
 * No return, only serves to list the testing_dir
 *************************************************************************************/
void exe_dir(){
	execl("/bin/ls", "ls", BASE_DIR, NULL);
	// If execl returns, an error occurred
	perror("> execl failed");
	exit(0);
}
