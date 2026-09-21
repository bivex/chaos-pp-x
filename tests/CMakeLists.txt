add_executable(test_c test_c.c)
target_link_libraries(test_c PRIVATE chaos::chaos)
target_compile_features(test_c PRIVATE c_std_99)
add_test(NAME test_c COMMAND test_c)

add_executable(test_cpp test_cpp.cpp)
target_link_libraries(test_cpp PRIVATE chaos::chaos)
target_compile_features(test_cpp PRIVATE cxx_std_11)
add_test(NAME test_cpp COMMAND test_cpp)

# Negative compilation tests (verify that ASSERT and ASSERT_MSG fail when cond is 0)
add_test(
    NAME test_assert_fails_on_false
    COMMAND ${CMAKE_C_COMPILER} -I${PROJECT_SOURCE_DIR} -x c -c -o /dev/null ${CMAKE_CURRENT_SOURCE_DIR}/fail_assert.c
)
set_tests_properties(test_assert_fails_on_false PROPERTIES WILL_FAIL TRUE)

add_test(
    NAME test_assert_msg_fails_on_false
    COMMAND ${CMAKE_C_COMPILER} -I${PROJECT_SOURCE_DIR} -x c -c -o /dev/null ${CMAKE_CURRENT_SOURCE_DIR}/fail_assert_msg.c
)
set_tests_properties(test_assert_msg_fails_on_false PROPERTIES WILL_FAIL TRUE)
