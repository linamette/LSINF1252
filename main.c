#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"




/******************************************************************************
*							                                                  *
*                          Variables globales		                          *
*							                                                  *
******************************************************************************/
int maxthreads;					//nombre de threads
int terminal;					//gère l'option "-"
int optionD;					//Print toutes les fractales ou non
fractal *best_fract;			//adresse vers la plus grande fractal

pthread_mutex_t mutex;			//mutex pour le buffer
pthread_mutex_t mutex2;			//mutex pour bestfractal et les variables globales
sem_t empty;					//semaphore pour le buffer vide
sem_t full;						//semaphore pour le buffer plein

fractal *buffer;				//buffer qui va stocker les fractales
int buffer_size;				//taille du buffer

int fractal_add = 0;			//nombre de fractales mises dans le buffer au total			
int fractal_compute = 0;		//nombre de fractales calculées
int fractal_pick = 0;			//nombres de fichiers fractales terminés




/* ***************************************************************************
*							                                                 *
*                           Gestion du buffer 		                         *
*							                                                 *
*****************************************************************************/
void insert (fractal fractal) {
	struct fractal *current = buffer;			//curseur pour parcourir le buffer
	int i;
	for (i = 0; fractal_get_height(current) != 0 && i < buffer_size; i++) {
		current++;
	}
	*current = fractal;
	fractal_add++;
}


struct fractal pick_fractal () {
	struct fractal *current = buffer;			//curseur pour parcourir le buffer
	fractal *fract = malloc(sizeof(fractal));				//fractal qui sera retournée
	int i;
	for (i = 0; fractal_get_height(current) == 0 && i < buffer_size; i++) {
		current++;
	}
	*fract = *current;
	fractal_set_height(current, 0);
	return *fract;
}




/* ***************************************************************************
*							                                                 *
*                          Producteur & consommateur 		                 *
*							                                                 *
*****************************************************************************/
void *consumer_function () {
	struct fractal *fract = (fractal*) malloc(sizeof(fractal));

	sem_wait(&full);							
	pthread_mutex_lock(&mutex);
	(*fract) = pick_fractal();				//prend une fractale dans le buffer				
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);								

	int i;		
	int j;									
	int width = fractal_get_width(fract);
	int height = fractal_get_height(fract);
	int calcul = 0;								//variable de la moyenne

	for (i = 0; i < width; i++) {				//boucle du calcul Julia et de la moyenne
		for (j = 0; j < height; j++) {
			fractal_compute_value(fract, i, j);
			calcul = calcul + fractal_get_value(fract, i, j);
			}
	}

	double average = (double) calcul / (double) (width*height);			//set de la moyenne pour la fractale courante
	fractal_set_average(fract, average);

	if (optionD != 0) {													//print ou pas toutes les fractales
		write_bitmap_sdl(fract, fractal_get_name(fract));
	}
	else {
	}

	pthread_mutex_lock(&mutex2);							//Change la bestfractal et change la variable d'arret
	if (fractal_get_average(best_fract) < average) {
		*best_fract = (*fract);
	}
	fractal_compute++;
	pthread_mutex_unlock(&mutex2);

	free(fract);
	return NULL;
}


void *producer_function (void *fileName) {
    FILE *file = NULL;
    char currentLine[100]="";		//utilisé par fgets suppose que les lignes des fichiers ne dépassent pas 100 char
    char *beginWord;				//détemine le début de chaque mots dans la ligne
  	char *beginLine = NULL;			//pointe vers el début de chaque ligne

    char *fractName ="";			//prépare un string pour le nom du fractale
    int fractHeigth;				//prépare un int pour la hauteur des fichiers
    int fractWidth;					//""                  "" largeur
    double fractA;					//partie réèlle
    double fractB;					//partie imaginaire

    file = fopen((char *)fileName,"r");			//initialisation du fichier courant

    if(file == NULL) {
    printf("une erreur s'est produite lors de l'initalisation de file\n");
    }

    else {
        //parcours le fichier ligne par ligne et crée une structure fractale
        while(fgets(currentLine, 100, file) != NULL) {     
           	if(currentLine[0] == '\n' || currentLine[0] == '#') {
              //on ignore cette ligne
            }
            else {
            	int i = 0;//compte les espaces
            	int j = 0;//curseur de caractères
            	beginWord ="";
            	beginLine = &currentLine[0];
            	//parcours la ligne en 'arretant à chaque espace pour enregister la donneé nécessaire
            	while(i < 5) {
                	if(*(beginLine+j) ==' ') {
                  		if(i == 0) { 
                    		fractName = beginWord;
                      		beginWord = "";
                      		//on a le nom du fractal
                      		i++;
                  		}
                  		else if (i == 1) {
                      		fractWidth = atoi(beginWord);
                      		beginWord="";
                      		//on a la hauteur du fractacle
                      		i++;
                  		}
                  		else if (i == 2) {
                     	 	fractHeigth = atoi(beginWord);
                      		beginWord = "";
                      		//on a la largeur du fractale
                      		i++;
                  		}
                  		else if (i == 3) {
                     		fractA = atof(beginWord);
                      		beginWord="";
                      		//on a la valeur de a
                      		i++;
                  		}
                  		else if (i == 4) {
                      		fractB = atof(beginWord);
                      		beginWord = "";
                      		// on a la valeur de b
                      		i++; 
                  		}
                  		else {
                      		//programmus réparo
                      		printf("probleme de cast\n");
                      		exit(EXIT_FAILURE);
                  		}
                	}
                	else {
                 	 	beginWord = add_char(beginWord,*(beginLine+j));
                	}
                	j++;
            	}
            	struct fractal fract = *fractal_new(fractName, fractWidth, fractHeigth, fractA, fractB);
            	sem_wait(&empty);
            	pthread_mutex_lock(&mutex);
            	insert(fract);
            	pthread_mutex_unlock(&mutex);
            	sem_post(&full);
            }
 											//////////////////////////    ATTENTION METHODE FREE     /////////////////////////
        }
        fclose(file);		//ferme le flux de file  
    }
    return NULL;
}


void *producer_function_stdin () {		//utilisé par fgets suppose que les lignes des fichiers ne dépassent pas 100 char
    char *beginWord;				//détemine le début de chaque mots dans la ligne
  	char *beginLine = NULL;	
  	char currentLine[100] = "";		//pointe vers el début de chaque ligne
  	char scan[3] = "oui";

    char *fractName ="";			//prépare un string pour le nom du fractale
    int fractHeigth;				//prépare un int pour la hauteur des fichiers
    int fractWidth;					//""                  "" largeur
    double fractA;					//partie réèlle
    double fractB;					//partie imaginaire

    int h;
	for (h = 0; strcmp(scan,"oui") == 0 && h < 10; h++) {
        //parcours le fichier ligne par ligne et crée une structure fractale
        	printf("Veuillez entrer une fractal : fractName width height a b\n");
			fgets(currentLine, 100, stdin);
			printf("Voulez-vous entrer une nouvelle fracal ? (oui ou non)\n");
			fgets(scan, 4, stdin);
        	fgets(currentLine, 100, stdin);  
           	int i = 0;//compte les espaces
            int j = 0;//curseur de caractères
            beginWord ="";
            beginLine = &currentLine[0];
            //parcours la ligne en 'arretant à chaque espace pour enregister la donneé nécessaire
            while(i < 5) {
              	if(*(beginLine+j) ==' ') {
                	if(i == 0) { 
                  		fractName = beginWord;
                  		beginWord = "";
                  		//on a le nom du fractal
                  		i++;
                	}
                	else if (i == 1) {
                  		fractWidth = atoi(beginWord);
                  		beginWord="";
                  		//on a la hauteur du fractacle
                  		i++;
                	}
                	else if (i == 2) {
                  		fractHeigth = atoi(beginWord);
                  		beginWord = "";
                  		//on a la largeur du fractale
                  		i++;
                	}
                	else if (i == 3) {
                  		fractA = atof(beginWord);
                  		beginWord="";
                  		//on a la valeur de a
                  		i++;
                	}
                	else if (i == 4) {
                  		fractB = atof(beginWord);
                  		beginWord = "";
                  		// on a la valeur de b
                  		i++; 
                	}
               		else {
                  		//programmus réparo
                  		printf("probleme de cast\n");
                  		exit(EXIT_FAILURE);
                	}
              	}
              	else {
                	beginWord = add_char(beginWord,*(beginLine+j));
              	}
              	j++;
            }
            struct fractal fract = *fractal_new(fractName, fractWidth, fractHeigth, fractA, fractB);
            sem_wait(&empty);
            pthread_mutex_lock(&mutex);
            insert(fract);
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            terminal = 0;
 											//////////////////////////    ATTENTION METHODE FREE     ////////////////////
    }
    return NULL;
}




/* ***************************************************************************
*							                                                 *
*                               Fonction main 		                         *
*							                                                 *
*****************************************************************************/
int main(int argc, char* argv[]) {

	/************ Gère les différentes entrées possibles ************/
    int files = argc-1;			//nombre d'entrée dans argv
    int thread = 0;				//emplacement dans argv de l'option maxthreads
	int count = 1;				//parcours de argv
	maxthreads = 1;				//nombre max de threads
	optionD = 0;				//gère l'option "-d"
	terminal = 0;				//gère l'option "-"

    while (count < files) {									//boucle de parcours des arguments
    	if (strcmp(argv[count], "--maxthreads") == 0) {
    		thread = count;
    		maxthreads = atoi(argv[count+1]);
    		if (maxthreads <= 0) {
    			maxthreads = 1;
    		}
    	}
    	if (strcmp(argv[count], "-d") == 0) {
    		optionD = count;
    	}
    	if (strcmp(argv[count], "-") == 0) {
    		terminal = count;
    	}
    	count++;
    }


    if (thread != 0) {			//calcul du nombre de fichiers
    	files = files-2;
   	}
   	if (optionD != 0) {
   		files = files-1;
   	}
   	if (terminal != 0) {
   		files = files-1;
   	}

   	int files2 = files;			//nombre de fichier + nom fichierOut
   	char *filesName[files];		//tableau contenant les différents noms de fichier
   	int i = 0;
   	int j = 1;
   	while (files > 0) {
   		if (thread != 0) {
   			if (j != thread && j != optionD && j != thread+1 && j != terminal) {
   				filesName[i] = argv[j];
   				i++;
   				files--;
   			}
   		}
   		else {
   			   	if (j != optionD && j != terminal) {
   				filesName[i] = argv[j];
   				i++;
   				files--;
   			}
   		}
   		j++;
   	}


   	/*********************** Lancement des threads *********************/
   	int producer = files2-1;										//nombre de threads producer à créer
   	int err;
   	buffer_size = 10+(2*maxthreads);								//taille du buffer	
   	buffer = (fractal*) malloc(buffer_size*sizeof(fractal));		//mise en mémoire du buffer

   	best_fract = (fractal*) malloc(sizeof(fractal));				//initialisation de bestfractal
   	fractal_set_average(best_fract, -8000);

   	pthread_mutex_init (&mutex, NULL);								//initialisation des mutex et semaphores
   	pthread_mutex_init (&mutex2, NULL);
   	sem_init (&empty, 0, buffer_size);
    sem_init (&full, 0, 0);
	pthread_t threads[producer+maxthreads];							//tableau de threads à lancer

   	struct fractal *fract = malloc(sizeof(fractal));
   	fractal_set_height(fract, 0);

   	for(i = 0;i < buffer_size;i++) {								//initialisation du buffer
		*(buffer+i) = *fract;
	}

	free(fract);

	if (terminal != 0) {											//lance l'ajout de fractales via le stdin
		producer_function_stdin();
	}

	for(i = 0;i < producer+maxthreads;i++) {						// crée tous les threads
		if(i<producer) {	
			err = pthread_create(&(threads[i]),NULL,(void*) &producer_function,(void*) filesName[i]);	
		}
		else {
			err = pthread_create(&(threads[i]),NULL,(void*) &consumer_function,NULL);
		}
		if(err != 0) {
			fprintf(stderr,"Erreur dans les threads\n");
			exit(EXIT_FAILURE);
		}
	}

	i = 0;
	while(1)
	{
		err = 1;
		if(i < producer && fractal_pick < producer)
		{
			err = pthread_tryjoin_np(threads[i],NULL);
		}
		else if(i >= producer)
		{
			err = pthread_tryjoin_np(threads[i],NULL);
		}
		if(err == 0)
		{
			if(i < producer)
			{
				fractal_pick++;
			}
			else // si thread de calcul
			{
				pthread_create(&(threads[i]),NULL,(void*) &consumer_function,NULL);
			}
		}
		if(fractal_pick < producer)
		{
			i = (i+1) % (producer+maxthreads);
		}
		else // tous les fichiers sont lus donc ne parcourt que les threads de calcul
		{
			i = producer + (i+1) % maxthreads;
		}
		if(fractal_pick == producer && fractal_compute == fractal_add && terminal == 0) {
			for(i = producer; i< producer+maxthreads; i++)
			{
				pthread_cancel(threads[i]);
			}
			write_bitmap_sdl(best_fract, filesName[files2-1]);
			pthread_mutex_destroy(&mutex);
			pthread_mutex_destroy(&mutex2);
			return 1;
		}
	}
}