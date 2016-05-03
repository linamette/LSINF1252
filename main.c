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

int maxthreads;
int optionD;

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

fractal *buffer;
int buffer_size;
fractal *best_fract;

pthread_t *consumer;
pthread_t *producer;


/* ***************************************************************************
*							                                                 *
*                           Gestion du buffer 		                         *
*							                                                 *
*****************************************************************************/

void insert (fractal fractal) {
	struct fractal *current = buffer;
	int i ;
	for (i = 0; fractal_get_height(&fractal) != 0 && i < buffer_size; i++) {
		current++;
	}
	*current = fractal;
}

struct fractal pick_fractal () {
	fractal *current = buffer;
	struct fractal fractal;
	int i;
	for (i = 0; current->width == 0 && i < buffer_size; i++) {
		current++;
	}
	fractal = *current;
	fractal_set_height(current, 0);
	return fractal;
}


/* ***************************************************************************
*							                                                 *
*                          Producteur & consommateur 		                 *
*							                                                 *
*****************************************************************************/

void *consumer_function () {
  printf("consummer lancé\n");
	fractal *fract;
	fract = (fractal*) malloc(sizeof(fractal));

  printf("avant tout va bien\n");
	sem_wait(&full);
	pthread_mutex_lock(&mutex);
	(*fract) = pick_fractal();
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);

  printf(" apres aussi optionD %d\n",optionD);		

	if (optionD != 0) {
		int i;
		int j;
		int write;
		int value;
		int width = fractal_get_width(fract);
		int height = fractal_get_height(fract);
		int calcul = 0;
		for (i = 0; i < width; i++) {
			for (j = 0; j < height; j++) {
				value = fractal_compute_value(fract, i, j);
				calcul = calcul + fractal_get_value(fract, i, j);
			}
		}

    	double average = (double) calcul / (double) (width*height);
		fractal_set_average(fract, average);

		printf("%d\n", height);
		int result = write_bitmap_sdl(fract, fractal_get_name(fract));
		printf("%d\n", height);
	}
	else {
		int i;
		int j;
		int write;
		int value;
		int width = fractal_get_width(fract);
		int height = fractal_get_height(fract);
		int calcul = 0;
		for (i = 0; i < width; i++) {
			for (j = 0; j < height; j++) {
				value = fractal_compute_value(fract, i, j);
				calcul = calcul + fractal_get_value(fract, i, j);
			}
		}

    	double average = (double) calcul / (double) (width*height);
		fractal_set_average(fract, average);

		if (fractal_get_average(best_fract) < average) {
			*best_fract = (*fract);
		}
		printf("%d\n", height);
		printf("%d\n", height);
	}
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

    printf("producer lancé\n");
    FILE *file = NULL;
    char currentLine[100]="";//utilisé par fgets suppose que les lignes des fichiers ne dépassent pas 100 char
    char *beginWord;//détemine le début de chaque mots dans la ligne
  	char *beginLine = NULL;//pointe vers el début de chaque ligne

    char *fractName ="";//prépare un string pour le nom du fractale
    int fractHeigth;//prépare un int pour la hauteur des fichiers
    int fractWidth;//""                  "" largeur
    double fractA;//partie réèlle
    double fractB;//partie imaginaire

    struct fractal fract;

    /*Initialisation du fichier courant*/
    file = fopen((char *)fileName,"r");

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
            fract = *fractal_new(fractName, fractWidth, fractHeigth, fractA, fractB);
            printf("initialisation terminée");
            sem_wait(&empty);
            pthread_mutex_lock(&mutex);
            insert(fract);
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
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

   	/******************* Lecture des fichiers ******************/
   	void *ptr;
   	int fileRunner = 0;
   	buffer_size = 2*maxthreads;
   	best_fract = (fractal*) malloc(sizeof(fractal));
   	fractal_set_average(best_fract, -8000);
   	pthread_mutex_init (&mutex, NULL);
   	sem_init (&empty, 0, buffer_size);
    sem_init (&full, 0, 0);
   	buffer = (fractal*) malloc(buffer_size*sizeof(fractal));
   	struct fractal fract = *fractal_new("empty_slot", 0, 0, 0.0, 0.0);
   	int err;
   	int x;
   	int y;
   	int z;
   	consumer = malloc((files2-1)*sizeof(pthread_t));
   	producer = malloc(maxthreads*sizeof(pthread_t));


   	for (x = 0; x < buffer_size; x++) {
   		*(buffer+x) = fract;
   	}

   	printf("Hello\n");

   	for (y =0; y < files2-1; y++) {
   		err = pthread_create(&producer[y], NULL, producer_function, (void*) filesName[y]);
		err = pthread_join(producer[y],NULL);
   	}

   	printf("producer\n");
   	for (z = 0; z < maxthreads; z++) {
   		err = pthread_create(&consumer[z], NULL, consumer_function, NULL);
		err = pthread_join(consumer[z],NULL);
   	}

   	printf("consumer\n");
  	if (optionD == 0) {
  		int result = write_bitmap_sdl(best_fract, filesName[files2-1]);
  	}
  	printf("consumer\n");
}//fin du main
