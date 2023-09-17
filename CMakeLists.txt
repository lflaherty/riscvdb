cmake_minimum_required(VERSION 3.20.0)

project(riscvdb LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(third_party)
add_subdirectory(src)

target_include_directories(riscvdb PUBLIC include)

target_compile_options(riscvdb PRIVATE -Wall -Wextra -Wpedantic -Werror)
