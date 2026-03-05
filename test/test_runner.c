#include "test_runner.h"
#include "brkman.h"
#include "heap.h" /* for analyzing chunk and heap metadata*/
#include "mem.h"  /* for heap reset */
#include <stdio.h>

const char* test_errmsgs[TEST_ERR_MSG_MAX] = {
    [TEST_ERR_ALLOC_RET_NULL] = "ALLOC_RET_NULL",
    [TEST_ERR_ALLOC_TOOSMALL] = "ALLOC_TOOSMALL",
    [TEST_ERR_ALLOC_NULLEXPCTD] = "ALLOC_NULLEXPCTD",
    [TEST_ERR_FREE_HEAP_NOTCOMPFREED] = "FREE_HEAP_NOTCOMPFREED",
};

bool test_alloc_01(const char** reterr_msg)
{
    const size_t mbytes = 32;
    const void* ptr = brkman_alloc(mbytes);

    if (NULL == ptr)
    {
        RETERR_MSG(TEST_ERR_ALLOC_RET_NULL);
        return false;
    }

    const brkman_chunk_t* header = brkman_heap_header_of(ptr);

    const bool memory_allocated = header->size >= mbytes;

    return memory_allocated;
}

bool test_alloc_02(const char** reterr_msg)
{
    const size_t mbytes = 1024;
    const void* ptr = brkman_alloc(mbytes);

    if (NULL == ptr)
    {
        RETERR_MSG(TEST_ERR_ALLOC_RET_NULL);
        return false;
    }

    const brkman_chunk_t* header = brkman_heap_header_of(ptr);

    const bool memory_allocated = header->size >= mbytes;

    return memory_allocated;
}

bool test_alloc_03(const char** reterr_msg)
{
    const size_t mbytes = SIZE_MAX;
    const void* ptr = brkman_alloc(mbytes);

    if (NULL == ptr)
    {
        return true;
    }

    RETERR_MSG(TEST_ERR_ALLOC_NULLEXPCTD);
    return false;
}

bool test_alloc_04(const char** reterr_msg)
{
    /* even if we multiply the header size by two here it's still big enough */
    const size_t mbytes = SIZE_MAX - (BRKMAN_CHUNK_HEADER_SIZE * 2);
    const void* ptr = brkman_alloc(mbytes);

    if (NULL == ptr)
    {
        return true;
    }

    RETERR_MSG(TEST_ERR_ALLOC_NULLEXPCTD);
    return false;
}

bool test_alloc_05(const char **reterr_msg)
{
    const size_t iterations = 100;
    const size_t mbytes = 1024;

    for(size_t run = 0; run < iterations; ++run)
    {
        const void* ptr = brkman_alloc(mbytes);

        if (NULL == ptr)
        {
            RETERR_MSG(TEST_ERR_ALLOC_RET_NULL);
            return false;
        }

        const brkman_chunk_t* header = brkman_heap_header_of(ptr);

        const bool memory_allocated = header->size >= mbytes;
        if(!memory_allocated)
        {
            RETERR_MSG(TEST_ERR_ALLOC_TOOSMALL);
            return false;
        }
    }
    return true;
}

bool test_free_01(const char** reterr_msg)
{
    const size_t mbytes = 1024;
    const brkman_heap_t* heap_ref = brkman_heap_ref();

    void* ptr = brkman_alloc(mbytes);
    if (NULL == ptr)
    {
        RETERR_MSG(TEST_ERR_ALLOC_RET_NULL);
        return false;
    }

    /* get the size of our chunk */
    const brkman_chunk_t* header = brkman_heap_header_of(ptr);
    const size_t chunksize = header->size;

    const size_t heap_size_after_alloc = heap_ref->free_mem_bytes;

    brkman_free(ptr);

    const size_t heap_size_after_free = heap_ref->free_mem_bytes;

    /* check if any memory was allocated */
    const size_t tmp_header_size = BRKMAN_CHUNK_HEADER_SIZE;
    const bool memory_was_allocated = chunksize >= (mbytes + tmp_header_size);
    /* check if the memory was fully freed */
    const size_t expsiz_after_free = heap_size_after_alloc + chunksize;
    const bool memory_was_freed = heap_size_after_free == expsiz_after_free;

    if (!memory_was_allocated)
    {
        RETERR_MSG(TEST_ERR_ALLOC_TOOSMALL);
        return false;
    }
    if (!memory_was_freed)
    {
        RETERR_MSG(TEST_ERR_FREE_HEAP_NOTCOMPFREED);
        return false;
    }
    return true;
}

bool test_free_02(const char** reterr_msg)
{
    UNUSED(reterr_msg);
    brkman_free(NULL);
    return true;
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
     "Here we try to allocate an enormous amount of memory to check the "
     "overflow behaviour.",
     test_alloc_03},
    {"TEST_ALLOC_04",
     "Here we try to allocate an enormous amount of memory to "
     "push the heap limits.",
     test_alloc_04},
    {"TEST_FREE_01", "We allocate some memory and try to free it immediately",
     test_free_01},
    {"TEST_FREE_02", "We try to call free with a NULL pointer", test_free_02},
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
        const char** current_test_result_errreason =
            &current_test_result->reason;
        /* save ref to current test case in test result */
        current_test_result->test_case = current_test_case;

        bool test_success =
            (current_test_case->func)(current_test_result_errreason);
        if (test_success)
        {
            current_test_result->result = TEST_RESULT_SUCCESS;
        }
        else
        {
            success = false;
            current_test_result->result = TEST_RESULT_FAILURE;
            fprintf(stderr, "current_test_result->reason: %s\n",
                    current_test_result->reason);
        }
        bool heap_reset_status = brkman_heap_reset();
        if (!heap_reset_status)
        {
            fprintf(
                stderr,
                "\033[35m\t --> failed to reset heap after test case\033[0m\n");
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
        const char* current_test_result_errreason = current_test_result->reason;

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
        if (current_test_result->result == TEST_RESULT_FAILURE)
        {
            fprintf(stderr, "\treason: \"%s\"\n",
                    current_test_result_errreason);
        }
    }
}

int main(void)
{
    bool all_tests_successful = test_runner();
    print_test_results();
    return (int) (all_tests_successful);
}
