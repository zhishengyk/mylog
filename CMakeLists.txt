cmake_minimum_required(VERSION 3.16)
project(mylog_mmap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MMAP_SRC
    src/mmap/mmap.cpp
    src/mmap/mmap.h
)

if (WIN32)
    list(APPEND MMAP_SRC src/mmap/mmap_win.cpp)
else()
    list(APPEND MMAP_SRC src/mmap/mmap_linux.cpp)
endif()

add_library(mmap STATIC ${MMAP_SRC})
target_include_directories(mmap PUBLIC src)

# Optional test target (only added if the test file exists)
set(MMAP_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_mmap.cpp)
if (EXISTS ${MMAP_TEST_SRC})
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(mmap_test ${MMAP_TEST_SRC})
    target_link_libraries(mmap_test PRIVATE mmap GTest::gtest_main)
    add_test(NAME mmap_test COMMAND mmap_test)
endif()
