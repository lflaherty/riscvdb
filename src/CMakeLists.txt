target_sources(riscvdb PRIVATE
    main.cpp
    riscv_processor.cpp
    console.cpp
    simhost.cpp
    fileloader.cpp
    memorymap.cpp
    linenoise_wrapper.cpp
)

add_subdirectory(commands)
