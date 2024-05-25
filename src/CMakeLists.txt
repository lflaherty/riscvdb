add_executable(riscvdb
    main.cpp
    console.cpp
    simhost.cpp
    fileloader.cpp
    memorymap.cpp
)

add_subdirectory(commands)

target_include_directories(riscvdb PRIVATE ${CXXOPTS_INCLUDE_DIR})
