#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fractal.h"

int main(int argc, char *argv[])
{
    char test1[]="-d";
    char test2[]="--maxthreads";

    const char *t1 = &test1[0];
    const char *t2 = &test2[0];
    const char *t3;

    int i = 0;


    while(i < argc)
    {
    	t3 = (const char *)argv[i];
    	if(strcmp(t1,t3) == 0)
    	{
    		/*si jamais l'argument courant est -d*/
    	}
    	else if(strcmp(t2,t3) == 0)
    	{
    		/*si jamais l'argument courant */
    	}
    	else if(*(argv[i]) = '-')
    	{
    		/*si jamais l'argument courant est '-' */
    	}
    	else
    	{
    		/*si non*/
    	}

    	i++;
    }
    return 0;
}

