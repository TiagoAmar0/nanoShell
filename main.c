/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @author name of author
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "debug.h"
#include "memory.h"

#define ARGS_SIZE 5

time_t start_time;

void handle_sigusr1(void);
void handle_signal(int signal);
char * read_user_input_line(void);
char ** split_input_into_arguments(char * line);

void handle_sigusr1(void){
	pid_t pid;
	int status;

	pid = fork();
	if(pid == -1){
		ERROR(5, "Failed to fork()");
	} else if(pid == 0){
		printf("\nnanoShell started at %s", ctime(&start_time));
		exit(EXIT_SUCCESS);
	} else {
		if(waitpid(pid, &status, 0) == -1){
			ERROR(6, "waitpid");
		}
	}
}

void handle_signal(int signal){
	int aux = errno;

	if(signal == SIGINT){
	}

	if(signal == SIGUSR1){
		handle_sigusr1();
	}

	errno = aux;
}

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

int main(int argc, char *argv[]){

	// Get start time
	time(&start_time);

	/* Disable warnings */
	(void)argc; (void)argv;

	char * line = NULL;
	char ** args = NULL;
	struct sigaction act;
	pid_t pid;
	int status;

	act.sa_handler = handle_signal;
	act.sa_flags = SA_RESTART;
	sigemptyset(&act.sa_mask);

	if(sigaction(SIGUSR1, &act, NULL) < 0){
		ERROR(4, "sigaction - SIGUSR1");
	}

	if(sigaction(SIGINT, &act, NULL) < 0){
		ERROR(4, "sigaction - SIGINT");
	}

	while(1){
		printf(">>> ");

		line = read_user_input_line();

		if(strcmp(line, "\n") == 0){
			continue;
		}

		args = split_input_into_arguments(line);
		if(strcmp(args[0], "bye") == 0){
			printf("[INFO] bye command detected. Terminating nanoShell.\n");
			return 0;
		}

		pid = fork();
		if(pid == -1){
			ERROR(1, "Failed to fork.");
		} else if(pid == 0){
			if(execvp(args[0], args) == -1){
				ERROR(1, "Failed to exec()");
			}
			exit(EXIT_SUCCESS);
		} else {
			if(waitpid(pid, &status, 0) == -1){
				ERROR(4, "Waitpid");
			}
		}
	}

	return 0;
}

