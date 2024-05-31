#include "fileloader.h"

#include <iostream>
#include <iomanip>
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

void ElfFileLoader::LoadMemory(SimHost& simHost)
{
    LoadProgramHeaders(simHost.Memory());
    LoadSymbols(simHost);
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

void ElfFileLoader::LoadProgramHeaders(MemoryMap& mem)
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
            std::cout << "LOAD  ";
            std::cout << "  VirtAddr = 0x";
            std::cout << std::hex << std::setw(8) << std::left << std::setfill(' ');
            std::cout << progHdr.p_paddr;
            std::cout << "  MemSize = 0x";
            std::cout << std::hex << std::setw(8) << std::left << std::setfill(' ');
            std::cout << progHdr.p_memsz;
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

void ElfFileLoader::LoadSymbols(SimHost& simHost)
{
    // find section string table
    Elf32_Off shStrTabOff = ((m_header.e_shstrndx) * m_header.e_shentsize) + m_header.e_shoff;
    Elf32_Shdr shStrTable;
    std::memcpy(&shStrTable, m_filebytes.data() + shStrTabOff,
                sizeof(shStrTable));
    // note: actual string table located at shStrTable.sh_offset

    bool strTableFound = false;
    bool symTableFound = false;
    Elf32_Shdr strTableHdr;
    Elf32_Shdr symTableHdr;
    std::memset(&strTableHdr, 0, sizeof(strTableHdr));
    std::memset(&symTableHdr, 0, sizeof(symTableHdr));


    std::cout << "Found sections:" << std::endl;

    Elf32_Off offset = m_header.e_shoff;
    for (Elf32_Half i = 0; i < m_header.e_shnum; ++i)
    {
        std::cout << i << " ";

        Elf32_Shdr sectionHdr;
        std::memcpy(&sectionHdr, m_filebytes.data() + offset,
                    sizeof(sectionHdr));

        offset += sizeof(sectionHdr);

        // get name:
        Elf32_Off nameOffset = shStrTable.sh_offset + sectionHdr.sh_name;
        std::byte c;
        while ((c = m_filebytes[nameOffset++]) != std::byte{'\0'})
        {
            std::cout << static_cast<char>(c);
        }

        std::cout << "\t";

        switch (sectionHdr.sh_type)
        {
            case SHT_STRTAB:
            {
                std::cout << "STRTAB";

                if (i == m_header.e_shstrndx)
                {
                    std::cout << "  (shstrtab)";
                    break;
                }

                if (strTableFound)
                {
                    throw std::runtime_error("multiple string tables in ELF");
                }

                strTableFound = true;
                strTableHdr = sectionHdr;
                break;
            }

            case SHT_SYMTAB:
            {
                std::cout << "SYMTAB";

                if (symTableFound)
                {
                    throw std::runtime_error("multiple symbol tables in ELF");
                }

                symTableFound = true;
                symTableHdr = sectionHdr;
                break;
            }

            default:
                std::cout << "(unknown: " << sectionHdr.sh_type << ")";
        }

        std::cout << std::hex;
        std::cout << "\toffset " << sectionHdr.sh_offset;
        std::cout << std::endl;
    }

    // TODO dynamic sections

    if (symTableFound && strTableFound)
    {
        std::cout << std::endl;
        std::cout << "Found symbols:" << std::endl;
        // read symbol table
        Elf32_Off symTableOffset = symTableHdr.sh_offset;
        for (Elf32_Word i = 0; i < symTableHdr.sh_entsize; ++i)
        {
            std::cout << std::dec << i << " ";

            Elf32_Sym symbol;
            std::memcpy(&symbol, m_filebytes.data() + symTableOffset,
                        sizeof(symbol));

            symTableOffset += sizeof(symbol);

            // TODO decode symbol and store in simHost
            (void)simHost;

            std::cout << std::endl;
        }
    }
}

} // namespace riscvdb
