/*************************************************************************************
 * myshell - student code for Project 1 of CSCE 489 
 *
 *************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shellfuncts.h"

int main(int argv, const char *argc[]) {
	(void) argv; // Make compile warnings go away - be sure to delete this line if you use the param
	(void) argc; // Make compile warnings go away - be sure to delete this line if you use the param

	//---TUTORIAL CODE---
	// If they just ran myshell, say Hello World--if they included a parameter, speak Australian
	/**
	if (argv == 1)
		hello(1);
	else
		hello(0);
	**/

	//---STARTUP MESSAGE---

	// going to make this a constant, since I'll likely need it later as a default case.
	const char* COMMANDS_STR = "-\tcreate <name>\n-\tupdate <name> <number> <text>\n-\tlist <name>\n-\tdir\n-\thalt\n";
	const int MAX_CHAR_LIMIT = 256;
	// used printf at first, but then realized a function existed for it already.
	send_msg("Welcome to my shell! The commands available are:\n\n");
	send_msg(COMMANDS_STR);

	//---LOGIC LOOP---
	// This is where the main logic of the program will take place. It should print the pid, prompt user for command,
	// parse command, fork a child process to execute the command, print pid of the child process, and wait for it if 
	// it is to be executed in the foreground.

	char input[MAX_CHAR_LIMIT];
	do {
		printf("\n\n\n----------\n[+] Shell PID: %d", getpid());

		// get the user input with fgets, since its more typically secure than alternatives (buffer overflow prevention.)
		send_msg("\n\n$ ");
		fflush(stdout);
		fgets(input, sizeof(input), stdin);
		input[strcspn(input, "\n")] = 0;

		// get the tokenized version of the input.  it will be NULL if the param count did not add up, or the command was incorrect.
		int is_amp = 0;
		
		char** words = parse_command(input, &is_amp); 
		if (words == NULL) {
			send_msg("> ERROR: Unknown command, or incorrect parameter count provided.  Try again.\n\n");
			send_msg(COMMANDS_STR);
			continue;
		}
		//originally had this as the while-loop condition, but it needs to be checked before we fork.
		if (strcmp(*words, "halt") == 0){
			break;	
		}
		
		//execute the command with fork
		exe_with_fork((const char**) words, is_amp);

		for (int i = 0; i < get_command_params(words[0]); i++) {
			free(words[i]);
		}
		free(words);


	} while (1);
	send_msg("\n> Terminating shell...\n"); 
	
	

}
