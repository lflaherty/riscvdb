#ifndef RISCVDB_FILELOADER_H
#define RISCVDB_FILELOADER_H

#include <string>

namespace riscvdb
{

class FileLoader {
protected:
    FileLoader();
};

class ElfFileLoader : public FileLoader {
public:
    ElfFileLoader(const std::string& path);

    static const std::string EXT;
};

} // namespace riscvdb

#endif  // RISCVDB_FILELOADER_H
