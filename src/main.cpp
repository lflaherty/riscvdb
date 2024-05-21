#include <iostream>
#include <filesystem>
#include "cxxopts.hpp"
#include "console.h"
#include "simhost.h"
#include "fileloader.h"

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

    riscvdb::SimHost simHost;

    if (result.count("executable"))
    {
        std::string pathStr = result["executable"].as<std::string>();
        std::filesystem::path path(pathStr);

        if (!std::filesystem::exists(path))
        {
            std::cerr << "File " << pathStr << " does not exist" << std::endl;
            return -1;
        }

        std::cout << "Loading executable " << pathStr << std::endl;

        std::string ext = path.extension();
        try
        {
            if (ext == riscvdb::ElfFileLoader::EXT) {
                riscvdb::ElfFileLoader elfFileLoader(pathStr);
                simHost.LoadFile(elfFileLoader);
            } else if (ext == std::string(".bin")) {
                std::cerr << "raw binaries not yet supported" << std::endl;
                return -1;
            } else {
                std::cerr << "unexpected filetype " << ext << std::endl;
                return -1;
            }
        }
        catch (std::runtime_error& err)
        {
            std::cerr << "failed to load file" << std::endl;
            std::cerr << err.what() << std::endl;
            return -1;
        }
    }

    riscvdb::Console console(simHost);
    return console.run();
}
