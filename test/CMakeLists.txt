set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/
    ${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include
)

set(
    TEST_SOURCES
    competitors/competitor_test.cpp
    competitors/competitor_test_binary_test_data.cpp
    data/data_generator_test.cpp
)

add_executable(imiTest ${TEST_SOURCES})

target_link_libraries(
    imiTest

    imib
    gtest_main
)
