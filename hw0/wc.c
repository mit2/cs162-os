#include <stdio.h>
#include <ctype.h>

#define IN 1
#define OUT 0
#define SIGOUT 'Q'

void doWordCount(FILE *fd, int *counter, int argc);

/* Specs
    Read innput from file stream, count newlines, words, chars. Return counter table */

int main(int argc, char *argv[]) {
	int counter[] = {0,0,0};
	FILE *fd;
		
	if(argc > 1) {
		fd = fopen(*++argv, "r");
		if(fd == NULL) {
			printf("Can't open the flle: %s", *argv);
		return 1;	
		}
		doWordCount(fd, counter, argc);
		fclose(fd);
	}
	else 
		doWordCount(stdin, counter, argc);
		

	printf("newlines: %d, words: %d, chars: %d", counter[0], counter[1], counter[2]);    	
	return 0;
}

void doWordCount(FILE *fd, int *counter, int argc){
	char ch;
	int wordCheck = OUT;
	
	while((ch = (argc == 1 ? getchar() : getc(fd))) != EOF){      // switch getc() to getchar() by func pointers
		if(ch == SIGOUT){		     // unefficient soluntion, but for now only
			++counter[1];
			return;
		}
		if(isspace(ch)){
			++counter[2];                // chars counter
			if(wordCheck){
				 ++counter[1];       // word counter
				wordCheck = OUT;
				}
			if(ch == '\n') ++counter[0]; // newline counter
			
		}
		else {
			if(!wordCheck) wordCheck = IN;
			++counter[2];                // chars counter
		}
	
	}
	return;

	/** CAsE:  SIGOUT FIX --- replace for Cntr-D SIG */
	// Quick Fix-1 for NO Signal INterapt Solution: 
	// to fix following inconviniens with different wc counts from file and stdio do:
	// > redirrect stdio to tempFile
	// > doWordCount from tempFile
	// > delete tempFile
	
	// Quick Fix-2: strore input in string and afrter fo redirent to doWorfCount()
	return;
}
