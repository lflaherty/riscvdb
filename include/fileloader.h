#ifndef RISCVDB_FILELOADER_H
#define RISCVDB_FILELOADER_H

#include <string>
#include <vector>

namespace riscvdb
{

class FileLoader {
protected:
    void LoadFile(const std::string& path);

    std::vector<std::byte> m_filebytes;
};

class ElfFileLoader : public FileLoader {
public:
    enum ElfClass
    {
        Elf32,
        Elf64,
    };

    ElfFileLoader(const std::string& path);

    ElfClass GetElfClass() const;

    static const std::string EXT;

private:
    // Checks whether m_filebytes contains a valid ELF file
    void LoadHeader();

    ElfClass m_elfClass;
};

} // namespace riscvdb

#endif  // RISCVDB_FILELOADER_H
