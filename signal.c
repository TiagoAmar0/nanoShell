#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "signal.h"
#include "debug.h"
#include "memory.h"

extern time_t start_time;
extern int applications_executions;
extern int stdout_redirections;
extern int stderr_redirections;

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
		printf("\nSIGINT detected\n");
	}

	if(signal == SIGUSR1){
		handle_sigusr1();
	}

    if(signal == SIGUSR2){
        handle_sigusr2();
    }

	errno = aux;
}

void handle_sigusr2(void){
    FILE * file = NULL;
    time_t t = time(NULL);
    char filename[50];
    struct tm tm = *localtime(&t);
    snprintf(filename, 50, "nanoShell_status_%d.%02d.%02d_%02dh%02d.%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    // Creates file in writing mode
    file = fopen(filename, "w");
    if(file == NULL){
        WARNING("Não foi possível criar o ficheiro de estado.");
    }

    if(fprintf(file, "%d executions of applications\n", applications_executions) < 0){
        WARNING("Erro de escrita no ficheiro de estado");
    }

    if(fprintf(file, "%d executions with STDOUT redir\n", stdout_redirections) < 0){
        WARNING("Erro de escrita no ficheiro de estado");
    }

    if(fprintf(file, "%d executions with STDERR redir", stderr_redirections) < 0){
        WARNING("Erro de escrita no ficheiro de estado");
    }

    fclose(file);
}
