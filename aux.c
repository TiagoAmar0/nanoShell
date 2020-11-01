#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>


#include "aux.h"
#include "debug.h"
#include "memory.h"

#define _GNU_SOURCE

// Checks if given filename corresponds to actual file
int check_if_file_exists(char * filename){
	// Check if file exists and has read permissions
	if(access(filename, F_OK | R_OK) == -1){
		ERROR(2, "[ERROR] cannot open file '%s'", filename);
	}

	return 0;
}

// Receives a string with a command and proceeds to validate and execute it
// Returns 0 if success or 2 if the command should be ignored
int process_input(char * line){
	char ** args = NULL;
	pid_t pid;
	int status;

	// skips the proccess if user just pressed ENTER
	if(strcmp(line, "\n") == 0){
		return 2;
	}

	// Validates the presence of single/double quotes and pipes
	if(validate_input(line) != 0){
		printf("[ERROR] Wrong request %s", line);
		return 2;
	}

	// Get an array with splitted args
	args = split_input_into_arguments(line);

	// If command "bye" is entered, finishes the program
	if(strcmp(args[0], "bye") == 0){
		printf("[INFO] bye command detected. Terminating nanoShell.\n");
		return 0;
	}

	// Create a child process to run command
	pid = fork();
	if(pid == -1){
		ERROR(1, "Failed to fork.");
	} else if(pid == 0){ // Child process
		// Execute the application
		image_subs_exec(args);
		exit(EXIT_SUCCESS);
	} else { // Parent process
		// Waits for the children to execute
		if(waitpid(pid, &status, 0) == -1){
			ERROR(4, "Waitpid");
		}	
	}

	return 0;
}

// Checks if the first non blank character is an '#'
int check_comment(char * line){
	int i = 0;

	// Loops string until a non blank character is found
	while(isspace(line[i])){
		i++;
	}

	// If first non-blank character is an '#' signals that is a comment line by returning 1
	if(line[i] == '#'){
		return 1;
	}
	return 0;
}

// Read the file specified in -f/--file option and executes commands
void read_commands_file(char * filename){
	FILE * file = NULL;
	char * line = NULL;
	ssize_t length;
	int executed = 1;
	size_t n;

	file = fopen(filename, "r");
	if(file == NULL){
		ERROR(1, "Failed to open file");
	}

	printf("[INFO] executing from file '%s'\n", filename);

	while((length = getline(&line, &n, file)) != -1){
		if(check_comment(line) == 0){
			printf("[command #%d]: %s", executed, line);
			executed++;
			process_input(line);
		}
	}
}

// Generate "signal.txt" file
void generate_signal_file(void){
	FILE * file = NULL;

	file = fopen("signal.txt", "w");
	if(file == NULL){
		WARNING("It was not possible to create the signal.txt file");
		return;
	}

	if(fprintf(file, "kill -SIGINT %d\nkill -SIGUSR1 %d\nkill -SIGUSR2 %d", getpid(), getpid(), getpid()) < 0){
		WARNING("Erro de escrita");
		return;
	}

	printf("[INFO] created file 'signal.txt'\n");
	fclose(file);
}

// Receives args and performs the system call to execute an application
void image_subs_exec(char ** args){
    if (execvp(args[0], args) == -1) {
        ERROR(7, "Failed to execute exec() system call");
    };
}

// returns the user input
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

// Receives the user input string and returns an array of parameters
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

// Validates if the input has single/double quotes or pipes
int validate_input(char * input){
	if(strchr(input, '\'') != NULL || strchr(input, '"') != NULL || strchr(input, '|') != NULL)
		return 1;

	return 0;
}