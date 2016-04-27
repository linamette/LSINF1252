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
    char **filesName;

    int count = 1;

    while (count <= argc) {
    	if (strcmp(argv[count], "--maxthreads") == 0) {
    		thread = count;
    		maxthreads = atoi(argv[count+1]);
    	}
    	if (strcmp(argv[count], "-d") == 0) {
    		optionD = count;
    	}
    	count++;
    }

    if (thread != 1) {
    	files = files-2;
   	}
   	if (optionD != 1) {
   		files = files-1;
   	}

   	int i = 0;
   	int j = 1;
   	while (files > 0) {
   		if (j != thread && j != optionD && j != thread+1) {
   			filesName[i] = argv[j];
   			i++;
   			j++;
   			files--;
   		}
   		j++;
   	}
}
