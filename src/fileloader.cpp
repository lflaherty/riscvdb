#include "fileloader.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "elf.h"

namespace riscvdb {

FileLoader::FileLoader(const std::string& pathStr)
: m_pathStr(pathStr)
{
    // empty
}

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

const std::string& FileLoader::PathStr() const
{
    return m_pathStr;
}

const std::string ElfFileLoader::EXT = ".elf";

ElfFileLoader::ElfFileLoader(const std::string& path)
: FileLoader(path)
{
    LoadFile(path);
    LoadHeader();
}

void ElfFileLoader::LoadMemory(MemoryMap& mem)
{
    std::cout << "Found program headers:" << std::endl;

    MemoryMap::AddrType loadedSize = 0;

    Elf32_Off offset = m_header.e_phoff;
    for (Elf32_Half i = 0; i < m_header.e_phnum; ++i)
    {
        std::cout << i << " ";

        Elf32_Phdr progHdr;
        std::memcpy(&progHdr, m_filebytes.data() + offset,
                    sizeof(progHdr));

        offset += sizeof(progHdr);

        if (progHdr.p_type == PT_LOAD)
        {
            std::cout << "LOAD";
            std::cout << std::hex;
            std::cout << "    VirtAddr = 0x" << progHdr.p_paddr;
            std::cout << "    MemSize = 0x" << progHdr.p_memsz;
            std::cout << std::endl;

            loadedSize += progHdr.p_memsz;

            // TODO probably don't need an intermediate copy...
            std::vector<std::byte> sectionData(progHdr.p_filesz);
            std::copy(m_filebytes.begin() + progHdr.p_offset,
                      m_filebytes.begin() + progHdr.p_offset + progHdr.p_filesz,
                      sectionData.begin());

            mem.Put(progHdr.p_paddr, sectionData);
        }
        else
        {
            std::cout << "(unused)" << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << std::dec;
    std::cout << "Loaded " << loadedSize << " B into memory" << std::endl;
    std::cout << std::endl;
}

ElfFileLoader::ElfClass ElfFileLoader::GetElfClass() const
{
    return m_elfClass;
}

void ElfFileLoader::LoadHeader()
{
    std::size_t headerSize = sizeof(m_header);

    if (m_filebytes.size() < headerSize)
    {
        throw std::runtime_error("invalid elf file: header too small");
    }

    std::memcpy(&m_header, m_filebytes.data(), headerSize);

    if (!(m_header.e_ident[EI_MAG0] == ELFMAG0 &&  // magic 0x7F
          m_header.e_ident[EI_MAG1] == ELFMAG1 &&  // 'E'
          m_header.e_ident[EI_MAG2] == ELFMAG2 &&  // 'L'
          m_header.e_ident[EI_MAG3] == ELFMAG3))   // 'F'
    {
        // identification bytes don't match
        throw std::runtime_error("invalid elf file: bad identification bytes");
    }

    unsigned char elfClass = m_header.e_ident[EI_CLASS];
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

    if (m_header.e_machine != EM_RISCV)
    {
        throw std::runtime_error("invalid elf file: not RISC V machine");
    }

    if (m_header.e_type != ET_EXEC)
    {
        throw std::runtime_error("invalid elf file: only executable files are supported");
    }
}

} // namespace riscvdb
