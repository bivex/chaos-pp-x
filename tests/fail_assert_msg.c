#include <chaos/preprocessor/debug/assert.h>

void trigger_assert_msg(void) {
    /* This must fail compilation because condition is 0 */
    CHAOS_PP_ASSERT_MSG(0, "Intentional assertion failure for testing");
}
