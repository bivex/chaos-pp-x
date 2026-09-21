add_executable(production_example
    production_trading_engine/src/main.cpp
)

target_include_directories(production_example PRIVATE
    production_trading_engine/include
)

target_link_libraries(production_example PRIVATE
    chaos::chaos
)

target_compile_features(production_example PRIVATE
    cxx_std_17
)
