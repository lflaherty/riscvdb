# cxxopts
target_include_directories(riscvdb PRIVATE cxxopts/include)

# linenoise
target_include_directories(riscvdb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/linenoise)
target_sources(riscvdb PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/linenoise/linenoise.c)
