#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

main(){
/*CASE 1; 
int staff = 7;
pid_t pid = fork();
printf("Staff is %d", staff);

if(pid == 0) staff = 6;

return; */

/*CASE 2:
int *staff = malloc(sizeof(int)*1);
*staff = 7;
pid_t pid = fork();
printf("Staff is %d", *staff);

if(pid == 0) *staff= 6;
return; */

pid_t pid = fork();
int status;
if (pid != 0){
wait(&status);
}
printf("pid: %d", pid);
printf("Exit status: %d\n", WEXITSTATUS(status));


}
