#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <unistd.h>
#include <CUnit/Basic.h>
#include <string.h>
#include "libfractal/fractal.h"
#include "libfractal/tools.c"

/*variables nécessaires aux tests unitiares*/

struct fractal *struct_test;
int *avg;
char*test_read_fractale; /*ftest 10 10 0.1 0.1*/

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

int test_avg(void)
{

	avg = (int *)malloc(sizeof(int));

	*avg = fractal_get_average((const struct fractal *)struct_test);

	return 0;
}

void test3(void)
{
	CU_ASSERT_EQUAL(*avg,1);
	//CU_ASSERT_EQUAL(1,1);
}

/*test de la lecture d'un fichier fractale*/

int test_reader(void)
{
	test_read_fractale = (char *)malloc(19*sizeof(char));
	test_read_fractale = "ftest 10 10 0.1 0.1";
}

void test4(void)
{
	
}

int free_init(void)
{
	free((void *)struct_test);
	free((void *)avg);
	free((void *)test_read_fractale);
	return 0;
}


int main(int argc, const char *argv[])
 
{	/*declaration des registres*/
	CU_pSuite t0 = NULL;
	CU_pSuite t1 = NULL;
	CU_pSuite t2 = NULL;
	//CU_pSuite t3 = NULL;
	//CU_pSuite t4 = NULL;
	//CU_pSuite t5 = NULL;
	/*vérification de l'initialisation de suite de tests*/
	if(CUE_SUCCESS != CU_initialize_registry())
	{
		return CU_get_error();
	}

	
	/*Initialisation des suites de tests.*/

	t0 = CU_add_suite("test0",initialize_test,NULL);
	t1 = CU_add_suite("test1",test_values,NULL);
	t2 = CU_add_suite("test2", test_avg,free_init);
	//t3 = CU_add_suite("test3",initialize_test4,NULL);
	//t4 = CU_add_suite("test4",initialize_test5,clean_test5);
	//t5 = CU_add_suite("test5",initialize_test6,clean_test6);

	if(NULL == t0 || NULL == t1 || NULL == t2)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if( (NULL == CU_add_test(t0,"test initialisation fractale",test1)) ||
		(NULL == CU_add_test(t1,"test de valeur",test2))			   ||
		(NULL == CU_add_test(t2,"test de moyenne",test3)))
 	{
 		CU_cleanup_registry();
		return CU_get_error();	
	}

	CU_basic_set_mode(CU_BRM_SILENT);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}
