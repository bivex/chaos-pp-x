#include <iostream>
#include <cassert>
#include <string>
#include <chaos/preprocessor.h>

int main() {
    // Arithmetic evaluation
    int val = CHAOS_PP_EXPR(CHAOS_PP_ADD(20, 22));
    assert(val == 42);

    // Tuple element access: (size, index, tuple)
    int elem1 = CHAOS_PP_TUPLE_ELEM(3, 1, (10, 20, 30));
    assert(elem1 == 20);

#if CHAOS_PP_VARIADICS
    // Variadic element access
    int elem2 = CHAOS_PP_VARIADIC_ELEM(2, 10, 20, 30, 40);
    assert(elem2 == 30);
#endif

    // Token concatenation and stringize
    std::string s = CHAOS_PP_STRINGIZE(CHAOS_PP_CAT(test_, cpp));
    assert(s == "test_cpp");

    // Assertions (condition = 1 must compile cleanly)
    CHAOS_PP_ASSERT(1);
    CHAOS_PP_ASSERT_MSG(1, "Should not trigger");

    std::cout << "C++ smoke tests passed successfully." << std::endl;
    return 0;
}
