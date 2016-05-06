#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <unistd.h>
#include <CUnit/Basic.h>
#include <string.h>
#include "../libfractal/fractal.h"
#include "../libfractal/tools.c"

/*variables nécessaires aux tests unitiares*/

struct fractal *struct_test;
int *avg;
char*test_read_fractale; /*ftest 10 10 0.1 0.1*/
char *test_at_line;
struct fractal *test_reader;
char *hashtag;
/*Méthode de test d'initialisation de la fracatale, inclus
les méthodes setters/getters*/

int initialize_test(void)
{

	struct_test = (struct fractal *)malloc(sizeof(struct fractal));

	char test_name[] = "test_fractale";

	int test_width = 10;
	int test_height = 10;

	double test_real = 1.0;
	double test_imag = 1.0;
    
	struct_test = fractal_new((const char *)&test_name[0], test_width, test_height, test_real, test_imag);

	return 0;
}

void test1(void)
{
	
	CU_ASSERT_EQUAL(fractal_get_width((const struct fractal*)struct_test), 10);
	CU_ASSERT_EQUAL(fractal_get_height((const struct fractal *)struct_test), 10);
	CU_ASSERT_EQUAL(fractal_get_a((const struct fractal *)struct_test), 1.0);
	CU_ASSERT_EQUAL(fractal_get_b((const struct fractal *)struct_test), 1.0);
	
}

/*initialise à 1 toutes les valeurs de values et test si celles ci ont bien été encodées*/

int test_values(void)
{
	int i = 0;
	int j = 0;

	while(i < 10)
	{
		while(j < 10)
		{
			fractal_set_value(struct_test, i,j,1);
			j++;
		}
		j = 0;
		i++;
	}
	return 0;
}

void test2(void)
{
	int i = 0; 
	int j = 0;
	while(i < 10)
	{
		while(j < 10)
		{
			CU_ASSERT_EQUAL(fractal_get_value((const struct fractal *)struct_test,i,j),1);
			j++;
		}
		j = 0;
		i++;
	}
}

/*test de la moyenne d'un fractale ( ne fonctionne pas )*/

int test_ashtag(void)
{
	FILE *filea = NULL;

	filea = fopen("ashtag.txt","r");

	hashtag = (char *)malloc(sizeof(char));

	fgets(hashtag,2,filea);

	return 0;

}

void test3(void)
{
	CU_ASSERT_EQUAL(*hashtag,'#');
}
/*test de la lecture d'un fichier fractale*/

int at_line_test(void)
{
	FILE *file = NULL;

	file = fopen("at_line.txt","r");

	test_at_line = (char *)malloc(2*sizeof(char));

	fgets(test_at_line,2,file);

	return 0;

}

void test4(void)
{
	CU_ASSERT_EQUAL(*test_at_line,'\n');
}

int test_loader(void) {

    FILE *file = NULL;
    char currentLine[100]="";		//utilisé par fgets suppose que les lignes des fichiers ne dépassent pas 100 char
    char *beginWord;				//détemine le début de chaque mots dans la ligne
  	char *beginLine = NULL;			//pointe vers el début de chaque ligne

    char *fractName ="";			//prépare un string pour le nom du fractale
    int fractHeigth;				//prépare un int pour la hauteur des fichiers
    int fractWidth;					//""                  "" largeur
    double fractA;					//partie réèlle
    double fractB;					//partie imaginaire

    file = fopen("test_reader.txt","r");//initialisation du fichier courant

    if(file == NULL) {
    printf("une erreur s'est produite lors de l'initalisation de file\n");
    return 1;
    }

    else {
        //parcours le fichier ligne par ligne et crée une structure fractale
        while(fgets(currentLine, 100, file) != NULL) {     
           	if(currentLine[0] == '#')
            {

            }
            else
            {

            test_reader = (fractal *)malloc(sizeof(fractal));
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
            test_reader = fractal_new(fractName, fractWidth, fractHeigth, fractA, fractB);
            }
 											//////////////////////////    ATTENTION METHODE FREE     /////////////////////////
        }
        fclose(file);//ferme le flux de file  
    }
    return 0;
}

void test5(void)
{
	CU_ASSERT_EQUAL(strcmp(test_reader->name,"test"),0);
	CU_ASSERT_EQUAL(fractal_get_width((const struct fractal*)test_reader), 50);
	CU_ASSERT_EQUAL(fractal_get_height((const struct fractal *)test_reader), 50);
	CU_ASSERT_EQUAL(fractal_get_a((const struct fractal *)test_reader), 1.0);
	CU_ASSERT_EQUAL(fractal_get_b((const struct fractal *)test_reader), 1.0);	
}

int free_init(void)
{
	free((void *)struct_test);
	free((void *)test_read_fractale);
	free((void *)test_at_line);
	free((void *)hashtag);
	free((void *)test_reader);

	return 0;
}


int main(int argc, const char *argv[])
 
{	/*declaration des registres*/
	CU_pSuite t0 = NULL;
	CU_pSuite t1 = NULL;
	CU_pSuite t2 = NULL;
	CU_pSuite t3 = NULL;
	CU_pSuite t4 = NULL;
	if(CUE_SUCCESS != CU_initialize_registry())
	{
		return CU_get_error();
	}

	
	/*Initialisation des suites de tests.*/

	t0 = CU_add_suite("test0",initialize_test,NULL);
	t1 = CU_add_suite("test1",test_values,NULL);
	t2 = CU_add_suite("test2", test_ashtag,NULL);
	t3 = CU_add_suite("test3",at_line_test,NULL);
	t4 = CU_add_suite("test4",test_loader,free_init);

	if(NULL == t0 || NULL == t1 || NULL == t2 || NULL == t3 || NULL == t4)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if( (NULL == CU_add_test(t0,"test initialisation fractale",test1)) ||
		(NULL == CU_add_test(t1,"test de valeur",test2))			   ||
		(NULL == CU_add_test(t2,"test de moyenne",test3)) 	           ||
		(NULL == CU_add_test(t3,"passage à la ligne",test4))		   ||
		(NULL == CU_add_test(t4,"chargement d'une ligne",test5)))
 	{
 		CU_cleanup_registry();
		return CU_get_error();	
	}

	CU_basic_set_mode(CU_BRM_SILENT);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}
