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
int optionD;					//Print toutes les fractales ou non
fractal *best_fract;			//adresse vers la plus grande fractal

pthread_mutex_t mutex;			//mutex pour le buffer
pthread_mutex_t mutex2;			//mutex pour bestfractal et varible globale
sem_t empty;					//semaphore pour le buffer
sem_t full;						//semaphore pour le buffer

fractal *buffer;				//buffer qui va stocker les fractals
int buffer_size;				//taille du buffer

int fractal_add = 0;			//nombre de fractal mises dans le buffer au total			
int computedone = 0;			//nombre de fractal calculées




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
	fractal *fract = malloc(sizeof(fractal));						//fractal qui sera retournée
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

	sem_wait(&full);							//prend une fractal dans le buffer
	pthread_mutex_lock(&mutex);
	(*fract) = pick_fractal();				
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);							//prend une fractal dans le buffer		

	int i;		
	int j;
	int value;									//calul de Julia
	int width = fractal_get_width(fract);
	int height = fractal_get_height(fract);
	int calcul = 0;								//Calcul de la moyenne

	for (i = 0; i < width; i++) {								//boucle de Julia et de la moyenne
		for (j = 0; j < height; j++) {
			value = fractal_compute_value(fract, i, j);
			calcul = calcul + fractal_get_value(fract, i, j);
			}
	}

	double average = (double) calcul / (double) (width*height);			//set de la moyenne
	fractal_set_average(fract, average);

	if (optionD != 0) {													//print ou pas toutes les fractals
		printf("%d\n", height);
		int result = write_bitmap_sdl(fract, fractal_get_name(fract));
		printf("%d\n", height);
	}
	else {
		printf("%d\n", height);
	}

	pthread_mutex_lock(&mutex2);						//Change la bestfractal et change la varible d'arret
	if (fractal_get_average(best_fract) < average) {
		*best_fract = (*fract);
	}
	computedone++;
	pthread_mutex_unlock(&mutex2);

	printf("%d\n", computedone);
	printf("%d\n", fractal_add);
	free(fract);
	return NULL;
}


/*ajoute un caractère à un mot*/
char* add_char(char *word, char add) {
  	size_t l = strlen(word);
  	char *stock = (char *)malloc(l +1+1);
  	strcpy(stock, word);
  	stock[l] = add;
  	stock[l+1] = '\0';
  	return stock;
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
            printf("initialisation terminée");
            sem_wait(&empty);
            pthread_mutex_lock(&mutex);
            insert(fract);
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
 											//////////////////////////    ATTENTION METHODE FREE     /////////////////////////
        }
        fclose(file);//ferme le flux de file  
    }
    return NULL;
}




/* ***************************************************************************
*							                                                 *
*                               Fonction main 		                         *
*							                                                 *
*****************************************************************************/
int main(int argc, char* argv[]) {

	/******* Gère les différentes entrées possibles *******/
	printf("salut\n");
    int files = argc-1;
    int thread = 0;
	int count = 1;
	maxthreads = 1;
	optionD = 0;

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
   	printf("Hello\n");

   	/******************* Lancement des threads ******************/
   	int producer = files2-1;
   	int err;
   	buffer_size = 2*maxthreads;
   	buffer = (fractal*) malloc(buffer_size*sizeof(fractal));

   	best_fract = (fractal*) malloc(sizeof(fractal));
   	fractal_set_average(best_fract, -8000);

   	pthread_mutex_init (&mutex, NULL);
   	pthread_mutex_init (&mutex2, NULL);
   	sem_init (&empty, 0, buffer_size);
    sem_init (&full, 0, 0);
	pthread_t threads[producer+maxthreads];

   	struct fractal *fract = malloc(sizeof(fractal));
   	fractal_set_height(fract, 0);

   	for(i = 0;i < buffer_size;i++)
	{
		*(buffer+i) = *fract;
	}

	free(fract);

	for(i = 0;i < producer+maxthreads;i++)
	{
		if(i<producer)
		{	
			err = pthread_create(&(threads[i]),NULL,(void*) &producer_function,(void*) filesName[i]);	
		}
		else
		{
			err = pthread_create(&(threads[i]),NULL,(void*) &consumer_function,NULL);
		}
		if(err != 0)
		{
			fprintf(stderr,"Erreur dans les threads\n");
			exit(EXIT_FAILURE);
		}
	}

	int fractal_pick = 0;
	int result;

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
		if(fractal_pick == producer && computedone == fractal_add) {
			for(i = producer; i< producer+maxthreads; i++)
			{
				pthread_cancel(threads[i]);
			}
			result = write_bitmap_sdl(best_fract, fractal_get_name(best_fract));
			pthread_mutex_destroy(&mutex);
			pthread_mutex_destroy(&mutex2);
			return 1;
		}
	}
}//fin du main
