set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-pthread -Wno-unknown-warning-option)
add_compile_options(-pthread -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-dollar-in-identifier-extension -Wno-unknown-pragmas -Wno-subobject-linkage -Wno-deprecated-dynamic-exception-spec)

# -Wno-deprecated-dynamic-exception-spec is needed for jemalloc

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-Weverything -Wshadow-all -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-documentation -Wno-padded -Wno-global-constructors -Wno-sign-conversion -Wno-exit-time-destructors -Wno-switch-enum -Wno-weak-vtables -Wno-double-promotion -Wno-covered-switch-default -Wno-unused-macros -Wno-newline-eof -Wno-missing-variable-declarations -Wno-weak-template-vtables -Wno-missing-prototypes -Wno-float-equal -Wno-return-std-move-in-c++11 -Wno-unreachable-code-break -Wno-undefined-func-template -Wno-unknown-warning-option -Wno-pass-failed -Wno-ctad-maybe-unsupported -Wno-header-hygiene -Wno-poison-system-directories)
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/src/
)

set(
    SOURCES
    competitors/art.hpp
    competitors/b_tree_google_abseil.hpp
    competitors/b_tree_tlx.hpp
    competitors/bb_tree.hpp
    competitors/hash_map_robin_hood_flat.hpp
    competitors/hash_map_robin_hood_node.hpp
    competitors/hash_map_tsl_robin.hpp
    competitors/hash_map_tsl_sparse.hpp
    competitors/judy.hpp
    competitors/simple_vector.hpp
    competitors/skip_list.hpp
    competitors/sorted_vector.hpp
    competitors/unordered_map.hpp
    data/column.hpp
    data/data_generator.hpp
    data/entry_generator.hpp
    data/key_generator.hpp
    data/load_binary_data.hpp
    data/lookup_generator.hpp
    data/random_key_generator.cpp
    data/random_key_generator.hpp
    data/tid_generator.hpp
    evaluation/memory_tracker.cpp
    evaluation/memory_tracker.hpp
    evaluation/report.cpp
    evaluation/report.hpp
    evaluation/report_exporter.cpp
    evaluation/report_exporter.hpp
    evaluation/timer.cpp
    evaluation/timer.hpp
    execution/benchmark_runner.cpp
    execution/benchmark_runner.hpp
    execution/cases/abstract_benchmark_case.hpp
    execution/cases/base_benchmark_case.hpp
    execution/cases/case_bulk_insert.hpp
    execution/cases/case_bulk_load.hpp
    execution/cases/case_equality_lookup.hpp
    execution/cases/case_erase.hpp
    execution/cases/case_insert.hpp
    execution/cases/case_range_lookup.hpp
    types.cpp
    types.hpp
)

# TODO(Marcel): If the frustration level gets lower and I wanna try to integrate absl:flat_hash_map again:
# https://github.com/abseil/abseil-cpp/issues/183#issuecomment-458453010
# set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
# set(THREADS_PREFER_PTHREAD_FLAG TRUE)
# find_package(Threads REQUIRED)
# and add Threads::Threads as library, as mentioned in the issue.

set(
    LIBRARIES

    BwTree
    cssl
    mass-tree
    phtree
    robin_hood
)

if (NOT APPLE)
    set(LIBRARIES ${LIBRARIES} atomic pthread)
endif()

add_library(imib SHARED ${SOURCES})

# suppress warnings from 3rd party libraries
include(${PROJECT_SOURCE_DIR}/cmake/TargetLinkLibrariesSystem.cmake)

if(NOT APPLE)
    # Do not use jemalloc on Mac as we cannot properly replace the allocator - see third_party/CMakeLists.txt for details.
    target_link_libraries(imib jemalloc)
    target_compile_definitions(imib PUBLIC IMIB_WITH_JEMALLOC)

    # Add the jemalloc headers to IMUB so that we can use jemalloc-specific functions
    target_include_directories(imib PUBLIC ${CMAKE_BINARY_DIR}/third_party/jemalloc/include)
endif()

target_link_libraries(imib ${LIBRARIES})
target_link_libraries_system(
  imib

  absl::container
  ARTSynchronized
  bb-tree
  judy
  nlohmann_json::nlohmann_json
  skip_list
  tlx
  tsl::robin_map
  tsl::sparse_map
)

# Configure imiBench
add_executable(imiBench main.cpp)

target_link_libraries(
    imiBench

    imib
)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()
