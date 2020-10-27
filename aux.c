#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "aux.h"
#include "debug.h"
#include "memory.h"

char * read_user_input_line(void){
	char * buffer = NULL;
	size_t buffer_size = 0;

	// Read line from stdin (user input) and allocates memory
	if(getline(&buffer, &buffer_size, stdin) == -1){
		// Don't return an error if user just sents an EOF
		if(feof(stdin)){
			exit(0);
		} else {
			ERROR(1, "Failed to allocate memory for user input. Exiting.\n");
		}
	}

	return buffer;
}

char ** split_input_into_arguments(char * line){

	int total = 0, args_size = ARGS_SIZE;
	char * arg = NULL;

	// Allocate memory for a string array
	char ** args = MALLOC(sizeof(char *) * ARGS_SIZE);
	if(args == NULL){
		ERROR(2, "Error allocating memory for input arguments. Exiting.\n");
	}

	// Places the pointer in input first word
	arg = strtok(line, " \t\r\n\a");

	while(arg != NULL){

		// Add arg to args list and increments the total
		args[total] = arg;
		total++;

		// If the total of args is equal or more than the size of args array, reallocates more memory
		if(total >= args_size){
			args_size += ARGS_SIZE;
			// Reallocate memory
			args = realloc(args, sizeof(char *) * args_size);
			if(args == NULL){
				ERROR(3, "Error allocating memory for input arguments. Exiting.\n");
			}
		}

		// Place the pointer to the next word
		arg = strtok(NULL, " \t\r\n\a");
	}

	// Last argument should be NULL
	args[total] = NULL;

	return args;
}
