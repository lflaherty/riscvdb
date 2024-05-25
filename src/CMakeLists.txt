target_sources(riscvdb PRIVATE
    main.cpp
    console.cpp
    simhost.cpp
    fileloader.cpp
    memorymap.cpp
    linenoise_wrapper.cpp
)

add_subdirectory(commands)
