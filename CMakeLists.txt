cmake_minimum_required(VERSION 3.20)

project(LLM-Butler VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(butler
    apps/cli/main.cpp
    src/cli/commands.cpp
    src/fs/filesystem.cpp
    src/fs/filesystem_ops.cpp
    src/fs/app_paths.cpp
)

target_include_directories(butler PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(butler PRIVATE /W4)
else()
    target_compile_options(butler PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_executable(butler_fs_tests
    tests/fs_tests.cpp
    src/fs/filesystem.cpp
    src/fs/filesystem_ops.cpp
    src/fs/app_paths.cpp
)

target_include_directories(butler_fs_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(butler_fs_tests PRIVATE /W4)
else()
    target_compile_options(butler_fs_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME butler_fs_tests COMMAND butler_fs_tests)
