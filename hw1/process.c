#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <termios.h>
#include <string.h>

#include "process.h"
#include "shell.h"
#include "parse.h"


/**
 * Executes the program pointed by process name
 */
void launch_process(char *proc_name, char *path, tok_t *argv) {
  /** YOUR CODE HERE */
	int cifd = 245, cofd = 246, cerrfd = 247;								// child in,out,err fd's rand values
	int status = 0, i = 0, bgflag = 0;
	char *buff;
	buff = malloc(strlen(path)+strlen(proc_name)+2);	  // alloc buff for str to created form two sustrings
	buff = strcpy(buff, path);
	buff = strcat(buff, "/");
	buff = strcat(buff,proc_name);
	//printf("Buff %s\n", buff);
	//printf("Bufflen %d\n", (int)strlen("a"));
	//exit(1);
	
	// Do '&' check in cmd line and set the flag for it
	while(argv[i] != NULL) i++;
	if(strcmp(argv[--i], "&") == 0) bgflag++;
	//printf("TEST bgflag: %d\n", bgflag);
	
	
	pid_t pid = fork();
	if(pid == 0){
		if (shell_is_interactive){
		/* Put the process into the process group and give the process group
			the terminal, if appropriate.
			This has to be done both by the shell and in the individual
			child processes because of potential race conditions. */

			setpgrp();
			if (tcgetpgrp(shell_terminal) == shell_pgid && !bgflag)							// do check for fg, set chld get terminal
				tcsetpgrp (shell_terminal, getpgrp());
			
			
			/* Set the handling for job control signals back to the default. */
			signal (SIGINT, SIG_DFL);
			signal (SIGQUIT, SIG_DFL);
			signal (SIGTSTP, SIG_DFL);
			signal (SIGTTIN, SIG_DFL);
			signal (SIGTTOU, SIG_DFL);
			signal (SIGCHLD, SIG_DFL);

			/* Set the standard input/output channels of the new process.
			 * Child dublicate shells i/o and close it, execv will start new proc, witch will inherit this i/o
			 */
			if (cifd != STDIN_FILENO)											
			{
				dup2 (cifd, STDIN_FILENO);
				printf("cifd: %d ", cifd);
				close (cifd);
			}
			if (cofd != STDOUT_FILENO)
			{
				dup2 (cofd, STDOUT_FILENO);
				close (cofd);
			}
			if (cerrfd != STDERR_FILENO)
			{
				dup2 (cerrfd, STDERR_FILENO);
				close (cerrfd);
			}
			

			// CURR DEBUG IT
			i++;	// i = 3 for 'sleep 20 & NULL'
			tok_t *argvbg = malloc(i * sizeof(tok_t));		// ? tok_t *
			for (int k = 0; k < i; k++) argvbg[k] = NULL;

			for( int j = 0; j < i-1; j++){
				argvbg[j] = argv[j];
				//printf("Argvbg %d: %s\n", j, argvbg[j]);
			}
			//printf("Argvbg %d: %s\n", 2, argvbg[2]);
			// CURR

			/* Exec the new process. Make sure we exit. */
			if(bgflag){
				execv(buff, argvbg); // filename: path + exec_image_name
				perror ("execv 'argvbg'");
			}
			else{
				execv(buff, argv); // filename: path + exec_image_name
				perror ("execv");
			}
			exit (1);																// as chld trmdted return status to prnt
		}
	}else{
		/* Parent */
		if (shell_is_interactive){
		/* Put the process into the process group and give the process group
			the terminal, if appropriate.
			This has to be done both by the shell and in the individual
			child processes because of potential race conditions. */
																				// chld pid returned to prnt
			setpgid (pid, pid);													// set for chld proc own process group ID, become procgrp leader
			if (tcgetpgrp(shell_terminal) == shell_pgid && !bgflag)							// do check for fg in shell context! also
				tcsetpgrp (shell_terminal, getpgid(pid));
			//printf("From Parent terminal fg  pgid: %d\n", tcgetpgrp(shell_terminal));
			//printf("From Parent terminal   pgid: %d\n", pid);

			// CURR
			if(bgflag){
				waitpid(pid, &status, WNOHANG);									// return immediatly, no wait
				tcsetpgrp (shell_terminal, shell_pgid);
			}else{
				waitpid(pid, &status, 0);
				tcsetpgrp (shell_terminal, shell_pgid);							// get back cntl to shell
			}
			// CURR
		}
	}

	free(buff);
						  
}

/**
 * Puts a process group with id PID into the foreground. Restores terminal
 * settings from *tmodes. Waits until the process with id PID exits or pauses.
 * Then, saves the current terminal settings into *tmodes. Finally, puts the
 * shell back into the foreground and restores terminal settings from
 * shell_tmodes.
 *
 *     pid
 *     cont   -- Send a SIGCONT to the process group to make it resume if it was
 *               paused.
 *     tmodes -- A pointer to some memory to save terminal settings for this
 *               process. If this process is a new process, you should copy
 *               settings from shell_tmodes to start out with.
 *               Can be NULL to ignore terminal settings.
 *
 */
void put_process_in_foreground(pid_t pid, bool cont, struct termios *tmodes) {
  int status;
  /* Put the job into the foreground. */
  tcsetpgrp(shell_terminal, pid);
  if (tmodes)
    tcsetattr(shell_terminal, TCSADRAIN, tmodes);
  /* Send the job a continue signal, if necessary. */
  if (cont && kill(-pid, SIGCONT) < 0)
    perror ("kill (SIGCONT)");
  /* Wait for the process to report. */
  waitpid(pid, &status, WUNTRACED);
  /* Put the shell back in the foreground. */
  tcsetpgrp(shell_terminal, shell_pgid);
  /* Restore the shell's terminal modes. */
  if (tmodes)
    tcgetattr(shell_terminal, tmodes);
  tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);
}

/**
 * Put a job in the background.
 *
 *     pid
 *     cont -- Send the process group a SIGCONT signal to wake it up.
 *
 */
void put_process_in_background(pid_t pid, bool cont) {
  /* Send the job a continue signal, if necessary. */
  if (cont && kill(-pid, SIGCONT) < 0)
    perror("kill (SIGCONT)");
}