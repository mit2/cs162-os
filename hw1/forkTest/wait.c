#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

// Cuplicated wait
int foo(n){
  if(n<2){ 
    printf("Last call to foo()\n");
    exit(n);
  }
  else{
    int v1, v2;
    pid_t pid = fork();
    if(pid == 0) foo(n-1);
    pid_t pid2 = fork();
    if(pid == 0) foo(n-2);

    waitpid(pid, &v1, 0);
    waitpid(pid2, &v2, 0);
    printf("Exit Status: %d\n", WEXITSTATUS(v1) + WEXITSTATUS(v2));
    exit(WEXITSTATUS(v1) + WEXITSTATUS(v2));




  }
}

main(){ foo(10); }
