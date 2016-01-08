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
	int chld_ifd, chld_ofd, chld_errfd;
	
	char *buff;
	buff = malloc(strlen(path)+strlen(proc_name));	  // alloc buff for str to created form two sustrings
	buff = strcpy(buff, path);
	buff = strcat(buff, "/");
	pid_t pid = fork();
	pid_t pgid;
	if(pid == 0){
		if (shell_is_interactive){
		/* Put the process into the process group and give the process group
			the terminal, if appropriate.
			This has to be done both by the shell and in the individual
			child processes because of potential race conditions. */
			pid = getpid ();
			if (pgid == 0) pgid = pid;
			setpgid (pid, pgid);
			if (tcgetpgrp(shell_terminal) == shell_pgid)							// do check for fg, set chld get terminal
				tcsetpgrp (shell_terminal, pgid);
			/* Set the handling for job control signals back to the default. */
			signal (SIGINT, SIG_DFL);
			signal (SIGQUIT, SIG_DFL);
			signal (SIGTSTP, SIG_DFL);
			signal (SIGTTIN, SIG_DFL);
			signal (SIGTTOU, SIG_DFL);
			signal (SIGCHLD, SIG_DFL);

			/* Set the standard input/output channels of the new process. */
			if (chld_ifd != STDIN_FILENO)											// COULD BE ERR HERE ....
			{
			dup2 (chld_ifd, STDIN_FILENO);
			close (chld_ifd);
			}
			if (chld_ofd != STDOUT_FILENO)
			{
			dup2 (chld_ofd, STDOUT_FILENO);
			close (chld_ofd);
			}
			if (chld_errfd != STDERR_FILENO)
			{
			dup2 (chld_errfd, STDERR_FILENO);
			close (chld_errfd);
			}

			/* Exec the new process. Make sure we exit. */
			execv(strcat(buff,proc_name), argv); // filename: path + exec_image_name
			perror ("execv");
			exit (1);																// as chld trmdted return status to prnt
		}
	}else{
		/* Parent */
		if (shell_is_interactive){
		/* Put the process into the process group and give the process group
			the terminal, if appropriate.
			This has to be done both by the shell and in the individual
			child processes because of potential race conditions. */
			//pid = getpid ();
			pgid = pid;																// chld pid returned to prnt
			setpgid (pid, pgid);													// set for chld proc own process group ID, become procgrp leader
			if (tcgetpgrp(shell_terminal) == shell_pgid)							// do chek for fg in shell context! also
				tcsetpgrp (shell_terminal, pgid);
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
