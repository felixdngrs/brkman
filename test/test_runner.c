#include "test_runner.h"
#include "brkman.h"
#include "mem.h" /* for heap reset */
#include <stdio.h>

bool test_alloc_01(void)
{
    void* ptr = brkman_alloc(32);
    return (ptr != NULL);
}

bool test_alloc_02(void)
{
    void* ptr = brkman_alloc(1024);
    return ptr != NULL;
}

bool test_alloc_03(void)
{
    void* ptr = brkman_alloc(SIZE_MAX);
    return (ptr != NULL);
}

bool test_free_01(void)
{
    return false;
}

bool test_free_02(void)
{
    return false;
}

bool test_free_03(void)
{
    return false;
}

test_case_t test_list[_TEST_MAX] = {
    {"TEST_ALLOC_01",
     "A simple valid allocation in which we try to allocate a small amount of "
     "memory",
     test_alloc_01},
    {"TEST_ALLOC_02",
     "A simple valid allocation in which we try to allocate a big amount of "
     "memory",
     test_alloc_02},
    {"TEST_ALLOC_03",
     "Here we try to allocate an enormous amount of memory to push the limits "
     "of the allocator",
     test_alloc_03},
    {"TEST_FREE_01", "We allocate some memory and try to free it immediately",
     test_free_01},
    {"TEST_FREE_02", "We try to call free with a NULL pointer", test_free_02},
    {"TEST_FREE_03",
     "We try to call free with a pointer that doesn't belong to the heap",
     test_free_03},
};

test_result_t test_results[_TEST_MAX];

/**
 * @brief Executes all registered test cases and records their results.
 *
 * This function iterates over all test cases defined in test_list,
 * executes each test function, and stores the result (success/failure)
 * in the test_results array.
 *
 * @note This function does not print the results. Use a separate function
 *       to generate a test report from test_results.
 * @return true if all tests were successful, false otherwise
 */
bool test_runner()
{
    bool success = true;
    for (test_id_e test_id = 0; test_id < _TEST_MAX; ++test_id)
    {
        test_case_t* current_test_case = &(test_list[test_id]);
        test_result_t* current_test_result = &(test_results[test_id]);

        /* save ref to current test case in test result */
        current_test_result->test_case = current_test_case;

        bool test_success = (current_test_case->func)();
        if (test_success)
        {
            current_test_result->result = TEST_RESULT_SUCCESS;
        }
        else
        {
            success = false;
            current_test_result->result = TEST_RESULT_FAILURE;
        }
    }
    return success;
}

void print_test_results(void)
{
    for (test_id_e test_id = 0; test_id < _TEST_MAX; ++test_id)
    {
        test_result_t* current_test_result = &(test_results[test_id]);
        test_case_t* current_test_case = current_test_result->test_case;
        if (current_test_result->result == TEST_RESULT_UNKNOWN)
        {
            fprintf(stdout, "\033[34m[NOT RUN]\033[0m\t");
        }
        else if (current_test_result->result == TEST_RESULT_FAILURE)
        {
            fprintf(stderr, "\033[31m[FAILED]\033[0m\t");
        }
        else if (current_test_result->result == TEST_RESULT_SUCCESS)
        {
            fprintf(stderr, "\033[32m[SUCCESS]\033[0m\t");
        }
        fprintf(stderr, "%16s - \"%s\"\n", current_test_case->name,
                current_test_case->descr);
        bool heap_reset_status = brkman_heap_reset();
        if (!heap_reset_status)
        {
            fprintf(
                stderr,
                "\033[35m\t --> failed to reset heap after test case\033[0m\n");
        }
    }
}

int main(void)
{
    bool all_tests_successful = test_runner();
    print_test_results();
    return (int) (all_tests_successful);
}
