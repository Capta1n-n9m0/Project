#include "haiku.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <CUnit/Basic.h>

/* Pointer to the file used by the tests. */
static FILE *temp_file = NULL;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
    if (NULL == (temp_file = fopen("temp.txt", "w+")))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
    if (0 != fclose(temp_file))
    {
        return -1;
    }
    else
    {
        temp_file = NULL;
        return 0;
    }
}

void testGetline(void)
{
    char *line = NULL;
    size_t number_of_characters = 16; // excluding '\0' character
    size_t len = 0;

    if (NULL != temp_file)
    {
        fprintf(temp_file, "testing_getline\n");
        rewind(temp_file); // set the file pointer at the beginning of the stream

        CU_ASSERT( number_of_characters == getline(&line, &len, temp_file) ); 
        // test against number of characters in the first
        // line of temp_file (i.e testing_getline\n) excluding NULL character
    }
}

bool compare_haikus(haiku h1, haiku h2)
{
    if (strcmp(h1.author, h2.author))
        return false;
    for (int i = 0; i < 3; i++)
        if (strcmp(h1.lines[i], h2.lines[i]))
            return false;
    return true;
}

bool compare_books(book b1, book b2)
{
    if (b1.size != b2.size)
        return false;
    for (int i = 0; i < b1.size; i++)
        if (!compare_haikus(b1.poems[i], b2.poems[i]))
            return false;
    return true;
}


void test(){
    book b;
    b.size = 2;
    b.poems = calloc(2, sizeof(haiku));
    b.poems[0] = (haiku){
        "Author",
        {"line1",
         "line2",
         "line3"}};
    b.poems[1] = (haiku){
        "Author",
        {"line1",
         "line2",
         "line3"}};
    book actuall = read_book(japanese);
    compare_books(b, actuall);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test of testGetline", testGetline))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
