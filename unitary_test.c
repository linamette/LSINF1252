#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <unistd.h>
#include <CUnit/Basic.h>
#include <string.h>
#include "libfractal/fractal.h"
#include "libfractal/tools.c"

int *test;
int *test1;
int *test2;
int *test3;
int *test4;
int *test5;
int *test6;

/*test 1 : vérifie si la méthode fractal_new crée bien une fractale avec les paramètres demandés*/
int initialisation_fractale(void)
{
	char test_initialize_name[] = "tesr_fractle";
	const char *arg_name = &test_initialize_name[0];

	int test_width = 100;
	int test_height = 100;

	double test_real = 1.0;
	double test_imag = 1.0;

	fractal *premiere = fractal_new(arg_name, test_width, test_height, test_real, test_imag);

	return 0;
}



int main(int argc, const char *argv[])
 
{	/*declaration des registres*/
	CU_pSuite t0 = NULL;
	CU_pSuite t1 = NULL;
	CU_pSuite t2 = NULL;
	CU_pSuite t3 = NULL;
	CU_pSuite t4 = NULL;
	CU_pSuite t5 = NULL;
	/*vérification de l'initialisation de suite de tests*/
	if(CUE_SUCCESS != CU_initialize_registry())
	{
		return CU_get_error();
	}

	
	/*Initialisation des suites de tests.*/

	t0 = CU_add_suite("test0",initialize_test1,clean_test1);
	t1 = CU_add_suite("test1",initialize_test2,clean_test2);
	t2 = CU_add_suite("test2",initialize_test3,NULL);
	t3 = CU_add_suite("test3",initialize_test4,NULL);
	t4 = CU_add_suite("test4",initialize_test5,clean_test5);
	t5 = CU_add_suite("test5",initialize_test6,clean_test6);

	if(NULL == t0 ||
	   NULL == t1 ||
	   NULL == t2 ||
	   NULL == t3 ||
	   NULL == t4 ||
	   NULL == t5)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if((NULL == CU_add_test(t0,"test allocation",test_allocation)) ||
           (NULL == CU_add_test(t1,"test correspondance",test_correspondance)) ||
           (NULL == CU_add_test(t2,"test de myfree",test_my_free)) ||
	   (NULL == CU_add_test(t3,"test négativité ",test_neg_arg)) ||
	   (NULL== CU_add_test(t4,"test overflow",test_over_heap)) ||
	   (NULL == CU_add_test(t5,"test calloc",test_calloc)))
 	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	CU_basic_set_mode(CU_BRM_SILENT);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}