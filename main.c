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
#include "args.h"
#include "aux.h"
#include "signal.h"

time_t start_time;
int applications_executions = 0, stdout_redirections = 0, stderr_redirections = 0;
struct gengetopt_args_info program_args;

int main(int argc, char *argv[]){

	// Get start time
	time(&start_time);

	// Parse gengetopt arguments
	if (cmdline_parser(argc, argv, &program_args) != 0){
		ERROR(1, "Can't parse gengetopt arguments");
	}

	// Check if -m/--max is set and the parameter is an integer bigger than zero
	if(program_args.max_given && program_args.max_arg <= 0){
		printf("[ERROR] invalid value '%d' for -m/--max\n", program_args.max_arg);
		exit(1);
	}

	if(program_args.signalfile_given){
		generate_signal_file();
	}

	if(program_args.max_given){
		printf("[INFO] terminates after %d commands\n", program_args.max_arg);
	}

	char * line = NULL;
	struct sigaction act;
	int commands_executed = 0, response;

	// Signal handlers definitions
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

	// 
	if(program_args.file_given){
		if(check_if_file_exists(program_args.file_arg) != 0){
		}
		read_commands_file(program_args.file_arg);
	}

	if(!program_args.file_given && !program_args.help_given){
		while(1){
			printf("nanoShell$");

			// Retrieves user input
			line = read_user_input_line();
			
			// Processes the given command and returns the status of the operation.
			// 0 = Success
			// 2 = invalid input. restart the process
			response = process_input(line);

			if(response == 0){ // Response = 0 means that the command was successfully executed

				// Increments the total of commands executed
				commands_executed++;

				// checks if -m/--max is set and if the max commands limit was reached
				if(program_args.max_given && commands_executed >= program_args.max_arg){
					printf("\n[END] Executed %d commands (-m %d)\n", program_args.max_arg, program_args.max_arg);
					exit(EXIT_SUCCESS);
				}
			} else if(response == 2){ // Response = 2 means that the loop iteration must be skipped
				continue;
			}
		}
	}
	

	return 0;
}

