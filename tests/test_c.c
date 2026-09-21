#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <chaos/preprocessor.h>

int main(void) {
    /* Token concatenation */
    int CHAOS_PP_CAT(foo, bar) = 123;
    assert(foobar == 123);

    /* Stringize */
    const char* str = CHAOS_PP_STRINGIZE(foobar);
    assert(strcmp(str, "foobar") == 0);

    /* Arithmetic: 15 + 27 = 42 */
    int sum = CHAOS_PP_EXPR(CHAOS_PP_ADD(15, 27));
    assert(sum == 42);

    /* Logical and branching */
    int branch_true = CHAOS_PP_IIF(1)(100, 200);
    int branch_false = CHAOS_PP_IIF(0)(100, 200);
    assert(branch_true == 100);
    assert(branch_false == 200);

    /* Comparison */
    int eq = CHAOS_PP_EXPR(CHAOS_PP_EQUAL(5, 5));
    int neq = CHAOS_PP_EXPR(CHAOS_PP_EQUAL(5, 6));
    assert(eq == 1);
    assert(neq == 0);

    printf("C smoke tests passed successfully.\n");
    return 0;
}
