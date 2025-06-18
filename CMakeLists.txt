cmake_minimum_required(VERSION 3.10)
project(LibraryManagementSystem)

set(CMAKE_CXX_STANDARD 17)

# Collect all source files
file(GLOB SOURCES
    src/*.cpp
    lib/sqlite3/sqlite3.c
)

add_executable(lms ${SOURCES})

target_include_directories(lms PRIVATE include utils lib/sqlite3)

# Uncomment these lines if you have test files
# add_executable(test_book tests/test_book.cpp)
# add_executable(test_user tests/test_user.cpp)
