add_executable(test_c test_c.c)
target_link_libraries(test_c PRIVATE chaos::chaos)
target_compile_features(test_c PRIVATE c_std_99)
add_test(NAME test_c COMMAND test_c)

add_executable(test_cpp test_cpp.cpp)
target_link_libraries(test_cpp PRIVATE chaos::chaos)
target_compile_features(test_cpp PRIVATE cxx_std_11)
add_test(NAME test_cpp COMMAND test_cpp)
