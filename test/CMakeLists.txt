cmake_minimum_required(VERSION 3.20.0)

project(riscvdb_test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(ROOT_DIR "${PROJECT_SOURCE_DIR}/..")
set(SRC_DIR "${ROOT_DIR}/src")

add_executable(riscvdb_test TestMemoryMap.cpp ${SRC_DIR}/memorymap.cpp)

target_include_directories(riscvdb_test PUBLIC ${ROOT_DIR}/include)

# Enable extra warnings
target_compile_options(riscvdb_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
# Optimize
target_compile_options(riscvdb_test PRIVATE -O3)
