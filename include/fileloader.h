#ifndef RISCVDB_FILELOADER_H
#define RISCVDB_FILELOADER_H

#include <string>
#include <vector>
#include "elf.h"
#include "memorymap.h"

namespace riscvdb
{

class FileLoader {
public:
    FileLoader(const std::string& pathStr);
    virtual void LoadMemory(MemoryMap& mem) = 0;

    const std::string& PathStr() const;

protected:
    void LoadFile(const std::string& path);

    const std::string& m_pathStr;
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

    void LoadMemory(MemoryMap& mem);

    ElfClass GetElfClass() const;

    static const std::string EXT;

private:
    // Checks whether m_filebytes contains a valid ELF file
    void LoadHeader();

    Elf32_Ehdr m_header;
    ElfClass m_elfClass;
};

} // namespace riscvdb

#endif  // RISCVDB_FILELOADER_H
