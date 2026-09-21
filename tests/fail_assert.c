#include <chaos/preprocessor/debug/assert.h>

void trigger_assert(void) {
    /* This must fail compilation because condition is 0 */
    CHAOS_PP_ASSERT(0);
}
