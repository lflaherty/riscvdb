#include "fileloader.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "elf.h"

namespace riscvdb {

void FileLoader::LoadFile(const std::string& path)
{
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);

    if(!ifs)
    {
        throw std::runtime_error(path + ": " + std::strerror(errno));
    }

    std::streampos end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::size_t size = std::size_t(end - ifs.tellg());

    if(size == 0)
    {
        throw std::runtime_error(path + ": file empty");
    }

    m_filebytes.resize(size);

    if(!ifs.read((char*)m_filebytes.data(), m_filebytes.size()))
    {
        throw std::runtime_error(path + ": " + std::strerror(errno));
    }
}

const std::string ElfFileLoader::EXT = ".elf";

ElfFileLoader::ElfFileLoader(const std::string& path)
{
    LoadFile(path);
    LoadHeader();
}

void ElfFileLoader::LoadHeader()
{
    Elf32_Ehdr header;
    std::size_t headerSize = sizeof(header);

    if (m_filebytes.size() < headerSize)
    {
        throw std::runtime_error("invalid elf file: header too small");
    }

    std::memcpy(&header, m_filebytes.data(), headerSize);

    if (!(header.e_ident[EI_MAG0] == ELFMAG0 &&  // magic 0x7F
          header.e_ident[EI_MAG1] == ELFMAG1 &&  // 'E'
          header.e_ident[EI_MAG2] == ELFMAG2 &&  // 'L'
          header.e_ident[EI_MAG3] == ELFMAG3))   // 'F'
    {
        // identification bytes don't match
        throw std::runtime_error("invalid elf file: bad identification bytes");
    }

    unsigned char elfClass = header.e_ident[EI_CLASS];
    switch (elfClass)
    {
        case ELFCLASS32:
            m_elfClass = Elf32;
            break;

        case ELFCLASS64:
            m_elfClass = Elf64;
            break;

        default:
            // invalid header value
            throw std::runtime_error("invalid elf file: invalid class");
    }

    if (header.e_machine != EM_RISCV)
    {
        throw std::runtime_error("invalid elf file: not RISC V machine");
    }

    if (header.e_type != ET_EXEC)
    {
        throw std::runtime_error("invalid elf file: only executable files are supported");
    }
}

} // namespace riscvdb
