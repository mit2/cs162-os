#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_quit(tok_t arg[]);
int cmd_help(tok_t arg[]);
int cmd_pwd(tok_t arg[]);
int cmd_cd(tok_t arg[]);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(tok_t args[]);												// Alex: ?

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_quit, "quit", "quit the command shell"},
  {cmd_pwd, "pwd", "print current working directory path"},
  {cmd_cd, "cd", "change to other directory"},
};

/**
 * Prints a helpful description for the given command
 */
int cmd_help(tok_t arg[]) {
  for (int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++) {
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;
}

/**
 * Prints the current working directory to standard output							// Alex#: code
 */
int cmd_pwd(tok_t arg[]) {
  /* Quick version 1
  int status;
  pid_t pid = fork();
  if(pid == 0) execv("/bin/pwd", (char *const*)(--arg));									// Alex#: doing --arg birng to the begining of vector as need
																					// to pass full args vector to commnd
																					// as for "cmd_table[fundex].fun(&tokens[1]);" bring 'tokens' to begin of array
  wait(&status);
  return(WEXITSTATUS(status));*/


  /* Quick version 2 */
  char *cwd = getcwd(NULL, 0);
  if(cwd != NULL){
	  printf("%s\n", cwd);
	  return 0;
  }else
	  return -1;

}

/**
 * cd that takes one argument, a directory name, and changes the current
 * working directory to that directory.												// Alex#: code
 */
int cmd_cd(tok_t arg[]) {
  /* Version 1: This one not worked out
  int status;
  pid_t pid = fork();
  if(pid == 0) execv("/bin/cd", (char *const*)--arg);
  wait(&status);
  return(WEXITSTATUS(status));*/

  /* Version 2: Did't changed cwd, looks like env vars is unique for each process
  char *new_path = (char *)arg; ///															/// means my code
  char *curr_path = getenv("PWD"); ///
  //printf("envir var: %s\n", curr_path); /// ok
  //printf("new_path var: %s\n", new_path[0]); /// ok
  // Doing simple verison of 'cd' b/c luck of time
  printf("envir var: %s\n", getenv("PWD")); /// ok
  setenv("PWD", "/home/vagrant", 1);
  printf("envir var: %s\n", getenv("PWD")); /// ok
  return;*/

  /* Version 3: */
  int chdir_status = chdir(*arg);													// dererence arg, as was passed as adress
  if(chdir_status){
	  printf("Error to change directory: %d\n", chdir_status);
	  return chdir_status;
  }
  else
	  return chdir_status;
}





/**
 * Quits this shell
 */
int cmd_quit(tok_t arg[]) {
  exit(0);
  return 1;
}

/**
 * Looks up the built-in command, if it exists.
 */
int lookup(char cmd[]) {
  for (int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++) {
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
  }
  return -1;
}

/**
 * Intialization procedures for this shell
 */
void init_shell() {
  /* Check if we are running interactively */
  shell_terminal = STDIN_FILENO;
  shell_is_interactive = isatty(shell_terminal);

  if(shell_is_interactive){
    /* Force the shell into foreground */
    while(tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

int shell(int argc, char *argv[]) {
  char *input_bytes;
  tok_t *tokens;
  int line_num = 0;
  int fundex = -1;

  init_shell();

  if (shell_is_interactive)
    /* Please only print shell prompts when standard input is not a tty */
    fprintf(stdout, "%d: ", line_num);

  while ((input_bytes = freadln(stdin))) {
    tokens = get_toks(input_bytes);
    fundex = lookup(tokens[0]);														// Alex# fundex -- get 1st input token and return func index in cmd_table
    if (fundex >= 0) {
      cmd_table[fundex].fun(&tokens[1]);											// Alex#: call func form cmd_table
    } else {
      /* REPLACE this to run commands as programs. */
      //fprintf(stdout, "This shell doesn't know how to run programs.\n");
	  int status;
      pid_t pid = fork();
      if(pid == 0) execv(tokens[0], tokens);
      wait(&status);
    }

    if (shell_is_interactive)
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);
  }

  return 0;
}
