#include <stdio.h>
#include <string.h>
#include <stdlib.h>

main(){
char a1[] = {'a','b','c'};						// is array, not a pointer
char *a2[] = {"Hello1", "Hello2", "Hello3", NULL};	// is array of pointers to string
char *p1 = a1;
char **p2 = a2;
int c, ind = 0;

int *ptr, i = 5;
ptr = &i;
*ptr = 6;
//printf("pointer: %d\n", *ptr);

char **pa = a2;
while(*pa != NULL) {
	printf("%s ", *pa);
	ind++;
	*pa++;
}
//printf("Value: %s i: %d\n", *--pa, ind);
printf("Char: %c\n", (*--pa)[5]);









/*// pOINTER IS VAR, AN DINCREMENT IT IS LEGAL. ARRAY IS  NOT VAR -- NOT LEGAL TO INCREMENT IT
printf("Arr1: %c\n", *++p1);
printf("Arr2: %s\n", *++p2);					
printf("Arr2: %c\n", (*++p2)[5]);				// print 5nd char from 2nd str
//(*p2)[5] = 'A';
printf("Arr2: %c\n", (*p2)[5]);					// print '3'
while(c = *++p2[0])printf("%c", c);				// printing 'ello3'
*/

return 0;
}

