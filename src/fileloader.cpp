#include "fileloader.h"

namespace riscvdb {

FileLoader::FileLoader()
{
    // empty
}

const std::string ElfFileLoader::EXT = ".elf";

ElfFileLoader::ElfFileLoader(const std::string& path)
{
    (void)path;
}

} // namespace riscvdb
