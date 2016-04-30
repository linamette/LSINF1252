#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "libfractal/fractal.h"
#include "libfractal/tools.c"

char* add_char(char *word, char add)
{
  	size_t l = strlen(word);
  	char *stock = (char *)malloc(l +1+1);
  	strcpy(stock, word);
  	stock[l] = add;
  	stock[l+1] = '\0';
  	return stock;
}

/************* Calcul de la fractal et transformation en Bitmap *********************/

void fractal_calcul (struct fractal *fractal) {
	int i;
	int j;
	int write;
	int value;
	int width = fractal_get_width(fractal);
	int height = fractal_get_height(fractal);

	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			value = fractal_compute_value(fractal, i, j);
		}
	}
	printf("%d\n", height);
	int result = write_bitmap_sdl(fractal, fractal_get_name(fractal));
	printf("%d\n", height);
}




/* **************************
*							*
*       Fonction main 		*
*							*
************************** */

int main(int argc, char* argv[]) {

	/******* Gère les différentes entrées possibles *******/

    int optionD = 0;
    int files = argc-1;
    int maxthreads = 1;
    int thread = 0;
	int count = 1;
	struct fractal fract;

    while (count < files) {
    	if (strcmp(argv[count], "--maxthreads") == 0) {
    		thread = count;
    		maxthreads = atoi(argv[count+1]);
    	}
    	if (strcmp(argv[count], "-d") == 0) {
    		optionD = count;
    	}
    	count++;
    }


    if (thread != 0) {
    	files = files-2;
   	}
   	if (optionD != 0) {
   		files = files-1;
   	}

   	int files2 = files;
   	char *filesName[files];
   	int i = 0;
   	int j = 1;
   	while (files > 0) {
   		if (thread != 0) {
   			if (j != thread && j != optionD && j != thread+1) {
   				filesName[i] = argv[j];
   				i++;
   				files--;
   			}
   		}
   		else {
   			   	if (j != optionD) {
   				filesName[i] = argv[j];
   				i++;
   				files--;
   			}
   		}
   		j++;
   	}

   	/******************* Lecture des fichiers ******************/

    int fileRunner = 0;    //Pour le parcours du tableau contenant les noms d e fichiers

    FILE *file = NULL;

    while(fileRunner < files2-1)
    {

        /*Initialisation du fichier courant*/
        file = fopen(filesName[fileRunner],"r");

        char currentLine[100]="";//utilisé par fgets suppose que les lignes des fichiers ne dépassent pas 100 char

        if(file == NULL)
        {
          printf("une erreur s'est produite lors de l'initalisation de file\n");
        }
        else
        {
          	char *beginWord;//détemine le début de chaque mots dans la ligne
          	char *beginLine = NULL;//pointe vers el début de chaque ligne

          	char *fractName ="";//prépare un string pour le nom du fractale
          	int fractHeigth;//prépare un int pour la hauteur des fichiers
          	int fractWidth;//""                  "" largeur
          	double fractA;//partie réèlle
          	double fractB;//partie imaginaire
      
          	//parcours le fichier ligne par ligne et crée une structure fractale
          	while(fgets(currentLine, 100, file) != NULL)
          	{     
            	int i = 0;//compte les espaces
            	int j = 0;//curseur de caractères
            	beginWord ="";
            	beginLine = &currentLine[0];
            	//parcours la ligne en 'arretant à chaque espace pour enregister la donneé nécessaire
            	while(i < 5)
            	{
              		if(*(beginLine+j) ==' ')
              		{
                		if(i == 0)
                		{ 
                  			fractName = beginWord;
                  			beginWord = "";
                  			//on a le nom du fractal
                  			i++;
                		}
                		else if (i == 1)
                		{
                  			fractWidth = atoi(beginWord);
                  			beginWord="";
                  			//on a la hauteur du fractacle
                  			i++;
                		}
                		else if (i == 2)
                		{
                  			fractHeigth = atoi(beginWord);
                  			beginWord = "";
                  			//on a la largeur du fractale
                  			i++;
                		}
                		else if (i == 3)
                		{
                  			fractA = atof(beginWord);
                  			beginWord="";
                  			//on a la valeur de a
                  			i++;
                		}
                		else if (i == 4)
                		{
                  			fractB = atof(beginWord);
                  			beginWord = "";
                  			// on a la valeur de b
                  			i++; 
                		}
               			else
                		{
                  			//programmus réparo
                  			printf("probleme de cast\n");
                  			return EXIT_FAILURE;
                		}
              		}
              		else
              		{
                		beginWord = add_char(beginWord,*(beginLine+j));
              		}
              		j++;
            	}
            	fract = *fractal_new(fractName, fractWidth, fractHeigth, fractA, fractB);
            	printf("init");
            	fractal_calcul(&fract);
            	printf("win");
          	}
          	fclose(file);//ferme le flux de file   
          	fileRunner++;
    	}
  	}
  	fractal_free(&fract);
}//fin du main
