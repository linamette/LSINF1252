#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "libfractal/fractal.h"

int main(int argc, char* argv[]) {

	/******************* Gère les différentes entrées possibles *******************/

    int optionD = 0;
    int files = argc-1;
    int maxthreads = 1;
    int thread = 0;

    //printf("%d\n", argc);

    int count = 1;

    while (count <= files) {
    	if (strcmp(argv[count], "--maxthreads") == 0) {
    		thread = count;
    		maxthreads = atoi(argv[count+1]);
    	}
    	if (strcmp(argv[count], "-d") == 0) {
    		optionD = count;
    	}
    	//printf("%s\n", argv[count]);
    	count++;
    }
    //printf("%d\n", thread);

    if (thread != 0) {
    	files = files-2;
   	}
   	if (optionD != 0) {
   		files = files-1;
   	}

   	char *filesName[files];
   	int i = 0;
   	int j = 1;
   	while (files > 0) {
   		if (j != thread && j != optionD && j != thread+1) {
   			filesName[i] = argv[j];
   			printf("%s\n", filesName[i]);
   			i++;
   			files--;
   		}
   		j++;
   	}
   	//printf("%d\n", optionD);
   	//printf("%d\n", maxthreads);	
   	return 0;

}
