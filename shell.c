/* SMP1: Simple Shell */

/* LIBRARY SECTION */
#include <ctype.h>              /* Character types                       */
#include <stdio.h>              /* Standard buffered input/output        */
#include <stdlib.h>             /* Standard library functions            */
#include <string.h>             /* String operations                     */
#include <sys/types.h>          /* Data types                            */
#include <sys/wait.h>           /* Declarations for waiting              */
#include <unistd.h>             /* Standard symbolic constants and types */

#include "smp1_tests.h"         /* Built-in test system                  */

/* DEFINE SECTION */
#define SHELL_BUFFER_SIZE 256   /* Size of the Shell input buffer        */
#define SHELL_MAX_ARGS 8        /* Maximum number of arguments parsed    */

/* VARIABLE SECTION */
enum { STATE_SPACE, STATE_NON_SPACE };	/* Parser states */


int imthechild(const char *path_to_exec, char *const args[])
{
	// TO-DO P5.1
	char *PATH = getenv("PATH"); 

	if(*path_to_exec == '/'){ //first case of an absolute path
		return execv(path_to_exec, args) ? -1 : 0;
	}else if(*path_to_exec == '.'){ //second case of .
		return execv(path_to_exec, args) ? -1 : 0;
	}else{ //case of ls

		char *token = strtok(PATH, ":"); 
		char* FULLPATH = (char *)malloc(255); 

		while(token != NULL){
			strcpy(FULLPATH, token); 
			strcat(FULLPATH, "/");
			strcat(FULLPATH, path_to_exec);
			if(!access(FULLPATH, X_OK)){
				return execv(FULLPATH, args) ? -1 : 0;
            }
            token = strtok(NULL, ":");
			
		}

		return -1;

	}

	return execv(path_to_exec, args) ? -1 : 0;
}

void imtheparent(pid_t child_pid, int run_in_background)
{
	int child_return_val, child_error_code;

	/* fork returned a positive pid so we are the parent */
	fprintf(stderr,
	        "  Parent says 'child process has been forked with pid=%d'\n",
	        child_pid);
	if (run_in_background) {
		fprintf(stderr,
		        "  Parent says 'run_in_background=1 ... so we're not waiting for the child'\n");
		return;
	}
	// TO-DO P5.4
	waitpid(child_pid, &child_return_val, 0);
	/* Use the WEXITSTATUS to extract the status code from the return value */
	child_error_code = WEXITSTATUS(child_return_val);
	fprintf(stderr,
	        "  Parent says 'wait() returned so the child with pid=%d is finished.'\n",
	        child_pid);
	if (child_error_code != 0) {
		/* Error: Child process failed. Most likely a failed exec */
		fprintf(stderr,
		        "  Parent says 'Child process %d failed with code %d'\n",
		        child_pid, child_error_code);
	}
}

/* MAIN PROCEDURE SECTION */
int main(int argc, char **argv)
{
	pid_t shell_pid, pid_from_fork;
	int n_read, i, exec_argc, parser_state, run_in_background;
	/* buffer: The Shell's input buffer. */
	char buffer[SHELL_BUFFER_SIZE];
	/* exec_argv: Arguments passed to exec call including NULL terminator. */
	char *exec_argv[SHELL_MAX_ARGS + 1];
	// TO-DO new variables for P5.2, P5.3, P5.6
	int counter = 1; //# of commands
	int shellLvl = 1; //first shell when incremented it will be a subshell within the first shell

	char **circularQ = (char**)malloc(9*sizeof(char*)); //allocates mem for circular qeueue

	for(int i =0; i< 9; i++){
		circularQ[i] = (char*)malloc(SHELL_BUFFER_SIZE*sizeof(char));
	}

	int reExe = 0;


	/* Entrypoint for the testrunner program */
	if (argc > 1 && !strcmp(argv[1], "-test")) {
		return run_smp1_tests(argc - 1, argv + 1);
	}

	/* Allow the Shell prompt to display the pid of this process */
	shell_pid = getpid();

	while (1) {
	/* The Shell runs in an infinite loop, processing input. */

		// TO-DO P5.2
		printf("Shell(pid=%d)%d> ", shell_pid, counter);
		fflush(stdout);

		/* Read a line of input. */
		if (fgets(buffer, SHELL_BUFFER_SIZE, stdin) == NULL)
			return EXIT_SUCCESS;
		n_read = strlen(buffer);
		run_in_background = n_read > 2 && buffer[n_read - 2] == '&';
		buffer[n_read - run_in_background - 1] = '\n';

		// TO-DO P5.3

		if(*buffer == '!'){
			reExe = (int)buffer[1]-48; //convert char to int using its ascii code
			if(reExe >= counter){ //trying to reexecute the same nth command as nth counter, or nth command greater than counter
				fprintf(stderr,"Not valid\n");
				continue;				
			}else{
				strcpy(buffer, circularQ[reExe % 9]);
				strcpy(circularQ[counter % 9], circularQ[reExe % 9]);
			}
		}else{
			strcpy(circularQ[counter % 9], buffer);
		}

		
		/* Parse the arguments: the first argument is the file or command *
		 * we want to run.                                                */

		parser_state = STATE_SPACE;
		for (exec_argc = 0, i = 0;
		     (buffer[i] != '\n') && (exec_argc < SHELL_MAX_ARGS); i++) {

			if (!isspace(buffer[i])) {
				if (parser_state == STATE_SPACE)
					exec_argv[exec_argc++] = &buffer[i];
				parser_state = STATE_NON_SPACE;
			} else {
				buffer[i] = '\0';
				parser_state = STATE_SPACE;
			}
		}

		/* run_in_background is 1 if the input line's last character *
		 * is an ampersand (indicating background execution).        */


		buffer[i] = '\0';	/* Terminate input, overwriting the '&' if it exists */

		/* If no command was given (empty line) the Shell just prints the prompt again */
		if (!exec_argc)
			continue;
		/* Terminate the list of exec parameters with NULL */
		exec_argv[exec_argc] = NULL;

		/* If Shell runs 'exit' it exits the program. */
		if (!strcmp(exec_argv[0], "exit")) {
			printf("Exiting process %d\n", shell_pid);
			return EXIT_SUCCESS;	/* End Shell program */

		} else if (!strcmp(exec_argv[0], "cd") && exec_argc > 1) {
		/* Running 'cd' changes the Shell's working directory. */
			/* Alternative: try chdir inside a forked child: if(fork() == 0) { */
			if (chdir(exec_argv[1]))
				/* Error: change directory failed */
				fprintf(stderr, "cd: failed to chdir %s\n", exec_argv[1]);	
			/* End alternative: exit(EXIT_SUCCESS);} */

		} else {
		/* Execute Commands */
			/* Try replacing 'fork()' with '0'.  What happens? */
			counter++;

			if(shellLvl >= 3 && !strcmp(exec_argv[0], "newsub")){
				fprintf(stderr, "Too deep!\n");
				continue;
			}
			
			pid_from_fork = fork();

			if (pid_from_fork < 0) {
				/* Error: fork() failed.  Unlikely, but possible (e.g. OS *
				 * kernel runs out of memory or process descriptors).     */
				fprintf(stderr, "fork failed\n");
				continue;
			}
			if (pid_from_fork == 0) {
				
				// TO-DO P5.6
				if(!strcmp(exec_argv[0], "newsub")){
					shellLvl++;
					counter = 1; //counter back to 1 since its a subshell

					shell_pid = getpid(); //child process becomes parent process for subshell
					continue;
				}
				return imthechild(exec_argv[0], &exec_argv[0]);
				/* Exit from main. */
			} else {
				imtheparent(pid_from_fork, run_in_background);
				/* Parent will continue around the loop. */
			}
		} /* end if */
	} /* end while loop */
	free(circularQ);
	return EXIT_SUCCESS;
} /* end main() */
