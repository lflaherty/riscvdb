#include <iostream>
#include "cxxopts.hpp"
#include "console.h"
#include "simhost.h"

int main(int argc, char* argv[])
{
    cxxopts::Options options("riscvdb", "RISC V simulator and debugger");
    options.add_options()
        ("executable", "The RISC V binary to execute", cxxopts::value<std::string>())
        ("h,help", "Print usage");
    options.parse_positional({"executable"});
    options.positional_help("riscv_binary_file");

    cxxopts::ParseResult result;
    try
    {
        result = options.parse(argc, argv);
    }
    catch (cxxopts::exceptions::exception& ex)
    {
        std::cerr << "error: " << ex.what() << std::endl;
        return -1;
    }

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (result.count("executable"))
    {
        std::string binFile = result["executable"].as<std::string>();
        // TODO use binFile
    }

    riscvdb::SimHost simHost;
    riscvdb::Console console(simHost);
    return console.run();
}
