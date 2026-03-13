#include <stdbool.h>

#define RETERR_MSG(id)                                                         \
    do                                                                         \
    {                                                                          \
        *reterr_msg = test_errmsgs[id];                                        \
    } while (0)

#define TEST_CASE_ACTIVE true
#define TEST_CASE_INACTIVE false

typedef enum test_errmsg_e
{
    TEST_ERR_ALLOC_RET_NULL,
    TEST_ERR_ALLOC_TOOSMALL,
    TEST_ERR_ALLOC_NULLEXPCTD,
    TEST_ERR_FREE_HEAP_NOTCOMPFREED,
    TEST_ERR_MSG_MAX
} test_errmsg_e;

extern const char* test_errmsgs[TEST_ERR_MSG_MAX];

/**
 * @enum test_id_e
 * @brief Enumeration of all available test case identifiers.
 *
 * This enum defines unique identifiers for each test case in the test suite.
 * It is used to index into the test_list and test_results arrays.
 */
typedef enum test_id_e
{
    TEST_ALLOC_01,
    TEST_ALLOC_02,
    TEST_ALLOC_03,
    TEST_ALLOC_04,
    TEST_ALLOC_05,
    TEST_FREE_01,
    TEST_FREE_02,
    _TEST_MAX
} test_id_e;

/**
 * @enum test_result_e
 * @brief Enumeration of possible test execution results.
 *
 * This enum represents the possible outcomes of a test case execution.
 */
typedef enum test_result_e
{
    TEST_RESULT_UNKNOWN,
    TEST_RESULT_FAILURE,
    TEST_RESULT_SUCCESS,
} test_result_e;

/**
 * @struct test_case_t
 * @brief Represents a single test case with metadata and execution function.
 *
 * This structure holds the name, description, and function pointer
 * for a single test case.
 */
typedef struct test_case_t
{
    bool active;       /**< Specifies whether the test should be executed. */
    const char* name;  /**< Short name/identifier of the test case. */
    const char* descr; /**< Detailed description of the test case. */
    bool (*func)(
        const char**); /**< Function pointer to the test implementation. */
} test_case_t;

/**
 * @struct test_result_t
 * @brief Stores the result of a test case execution.
 *
 * This structure associates a test case with its execution result.
 */
typedef struct test_result_t
{
    test_case_t* test_case; /**< Pointer to the executed test case. */
    test_result_e result;   /**< Result of the test execution. */
    const char* reason;     /**< Error reason (if an error occured). */
} test_result_t;

/**
 * @var test_list
 * @brief Global array of all test cases in the test suite.
 *
 * This array contains the metadata and function pointers for all test cases.
 * It is indexed by the test_id_e enum.
 */
extern test_case_t test_list[_TEST_MAX];

/**
 * @var test_results
 * @brief Global array to store the results of test case executions.
 *
 * This array stores the results of each test case after execution.
 * It is indexed by the test_id_e enum.
 */
extern test_result_t test_results[_TEST_MAX];
