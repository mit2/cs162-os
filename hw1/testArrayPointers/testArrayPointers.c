#include <stdio.h>
#include <string.h>
#include <stdlib.h>

main(){
char a1[] = {'a','b','c'};						// is array, not a pointer
char *a2[] = {"Hello1", "Hello2", "Hello3"};	// is array of pointers to string
char *p1 = a1;
char **p2 = a2;
int c;

int *ptr, i = 5;
ptr = &i;
*ptr = 6;
printf("pointer: %d\n", *ptr);


// pOINTER IS VAR, AN DINCREMENT IT IS LEGAL. ARRAY IS  NOT VAR -- NOT LEGAL TO INCREMENT IT
printf("Arr1: %c\n", *++p1);
printf("Arr2: %s\n", *++p2);					
printf("Arr2: %c\n", (*++p2)[5]);				// print 5nd char from 2nd str
//(*p2)[5] = 'A';
printf("Arr2: %c\n", (*p2)[5]);					// print '3'
while(c = *++p2[0])printf("%c", c);				// printing 'ello3'




/*char syspath[] = "/code/personal:/code/group";	// code with errors to test take time
char **pathdirs = malloc(strlen(syspath));
int i = 0;
	  // parse syspath stirng into tokens by ':' delim 
	  for(; i <= strlen(syspath); i++){
		  if(syspath[i] == ':') *pathdirs++;
		  *pathdirs[0]++ = syspath[i];
		  
	  }
	  printf("Pathdir: %s\n", *++pathdirs);*/
return 0;
}

