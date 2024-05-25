cmake_minimum_required(VERSION 3.20.0)

project(riscvdb LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(riscvdb)

add_subdirectory(third_party)
add_subdirectory(src)

target_include_directories(riscvdb PUBLIC include)

# Enable extra warnings
target_compile_options(riscvdb PRIVATE -Wall -Wextra -Werror)
# Optimize
target_compile_options(riscvdb PRIVATE -O3)
