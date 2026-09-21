cmake_minimum_required(VERSION 3.20)

project(chaos-pp
    VERSION 1.0.0
    DESCRIPTION "Chaos Preprocessor Library - Generative macro metaprogramming framework for C and C++"
    HOMEPAGE_URL "http://chaos-pp.sourceforge.net"
    LANGUAGES C CXX
)

# Header-only interface library
add_library(chaos INTERFACE)
add_library(chaos::chaos ALIAS chaos)
add_library(chaos::preprocessor ALIAS chaos)

target_include_directories(chaos INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(CMakeDependentOption)
option(CHAOS_ENABLE_TESTING "Build Chaos Preprocessor smoke tests" ${PROJECT_IS_TOP_LEVEL})
option(CHAOS_ENABLE_EXAMPLES "Build Chaos Preprocessor production examples" ${PROJECT_IS_TOP_LEVEL})
option(CHAOS_ENABLE_DOCS "Enable documentation generation target" ${PROJECT_IS_TOP_LEVEL})
option(CHAOS_INSTALL "Generate install targets for Chaos Preprocessor" ${PROJECT_IS_TOP_LEVEL})

if(CHAOS_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CHAOS_ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CHAOS_ENABLE_DOCS)
    find_program(XSLTPROC_EXECUTABLE xsltproc)
    if(XSLTPROC_EXECUTABLE)
        add_custom_target(docs
            COMMAND ${CMAKE_COMMAND} -E echo "Building documentation..."
            COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/pp-book/build
                ${CMAKE_CURRENT_SOURCE_DIR}/documentation/library.xml
                ${CMAKE_CURRENT_SOURCE_DIR}/documentation
                ${CMAKE_CURRENT_SOURCE_DIR}/built-docs
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pp-book
            COMMENT "Generating HTML documentation using xsltproc"
        )
    else()
        message(STATUS "xsltproc not found. Documentation target 'docs' will not be available.")
    endif()
endif()

if(CHAOS_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS chaos
        EXPORT chaosTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(DIRECTORY chaos
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
            PATTERN "*.h"
    )

    install(EXPORT chaosTargets
        FILE chaosTargets.cmake
        NAMESPACE chaos::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chaos
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/chaosConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/chaosConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/chaosConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chaos
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/chaosConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/chaosConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chaos
    )
endif()
