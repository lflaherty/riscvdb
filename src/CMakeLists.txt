add_executable(riscvdb main.cpp console.cpp)

add_subdirectory(commands)

target_include_directories(riscvdb PRIVATE ${CXXOPTS_INCLUDE_DIR})
