#define _GNU_SOURCE
#include "queue.h"
#include "haiku.h"
#include "server.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include <CUnit/Basic.h>

bool compare_haikus(haiku h1, haiku h2)
{
    if (strcmp(h1.author, h2.author))
        return false;
    for (int i = 0; i < 3; i++)
        if (strcmp(h1.lines[i], h2.lines[i]))
        {
            return false;
        }
    return true;
}

bool compare_books(book b1, book b2)
{
    if (b1.size != b2.size)
        return false;
    for (int i = 0; i < b1.size; i++)
        if (!compare_haikus(b1.poems[i], b2.poems[i]))
        {
            return false;
        }
    return true;
}

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

int init_suite_for_threads(void)
{
    return 0;
}

void *dummy_thread(void *arg){
    return NULL;
}

void test_threads(void){

    pthread_t dummy;
    // check the creation of two threads for writer and reader threads
    CU_ASSERT(pthread_create(&dummy, NULL, dummy_thread, NULL) == 0);

    CU_ASSERT(pthread_kill(dummy, SIGCHLD) == 0);

    CU_ASSERT(pthread_join(dummy, NULL) == 0);
}

int clean_suite_for_threads(void)
{
    return 0;
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

void testMsgQueue(void)
{

    int test_id_queue;

    CU_ASSERT(-1 != (test_id_queue = create_queue()));
    CU_ASSERT(-1 != (test_id_queue = access_queue()));
    CU_ASSERT(-1 != (remove_queue(test_id_queue)));
}

void testGetline(void)
{
    char *line = NULL;
    size_t number_of_characters = sizeof("testing_getline\n") - 1; // excluding '\0' character
    size_t len = 0;

    if (NULL != temp_file)
    {
        fprintf(temp_file, "testing_getline\n");
        rewind(temp_file); // set the file pointer at the beginning of the stream

        CU_ASSERT(number_of_characters == (getline(&line, &len, temp_file)));
        // test against number of characters in the first
        // line of temp_file (i.e testing_getline\n) excluding NULL character
    }
}

void testReadBook(void)
{
    haiku six_haikus[6] = {
        {"Matsuo Basho (1644-1694)", {"Autumn moonlight –", "a worm digs silently", "into the chestnut."}},
        {"Matsuo Basho (1644-1694)", {"Old pond", "a frog jumps", "the sound of water"}},
        {"Kobayashi Issa (1763-1828)", {"Autumn wind –", "mountain’s shadow", "wavers."}},
        {"Kobayashi Issa (1763-1828)", {"Don’t weep, insects –", "Lovers, stars themselves,", "Must part"}},
        {"Masaoka Shiki (1867-1902)", {"In the coolness", "of the empty sixth-month sky...", "the cuckoo’s cry."}},
        {"Masaoka Shiki (1867-1902)", {"the tree cut,", "dawn breaks early", "at my little window"}}};

    book test_haiku_book = {6, six_haikus};

    CU_ASSERT_TRUE(compare_books(read_category(japanese), test_haiku_book));
}

void test()
{
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
    book actuall = read_category(japanese);
    compare_books(b, actuall);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{

    CU_pSuite pSuite = NULL;

    CU_pSuite threads_Suite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    threads_Suite = CU_add_suite("Suite_for_checking_threads", init_suite_for_threads, clean_suite_for_threads);

    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test of testGetline", testGetline) ||
        NULL == CU_add_test(pSuite, "testing msg queue functions", testMsgQueue) ||
        NULL == CU_add_test(pSuite, "testing read_category function", testReadBook) ||
        NULL == CU_add_test(threads_Suite, "testing reader and writer threads", test_threads) )
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
