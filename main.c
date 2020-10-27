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
#include "aux.h"
#include "signal.h"

time_t start_time;
int applications_executions = 0, stdout_redirections = 0, stderr_redirections = 0;

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

	if(sigaction(SIGUSR2, &act, NULL) < 0){
		ERROR(4, "sigaction - SIGUSR2");
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

