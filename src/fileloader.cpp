#include "fileloader.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <sstream>
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
    std::cout << "  ";
    std::cout << std::setw(4) << std::setfill(' ') << std::left << "Num";
    std::cout << std::setw(8) << std::setfill(' ') << std::left << "Type";
    std::cout << std::setw(11) << std::setfill(' ') << std::left << "VirtAddr";
    std::cout << std::setw(11) << std::setfill(' ') << std::left << "MemSiz";
    std::cout << std::endl;

    MemoryMap::AddrType loadedSize = 0;

    Elf32_Off offset = m_header.e_phoff;
    for (Elf32_Half i = 0; i < m_header.e_phnum; ++i)
    {
        std::cout << "  ";
        std::cout << std::setw(4) << std::setfill(' ') << std::left << i;

        Elf32_Phdr progHdr;
        std::memcpy(&progHdr, m_filebytes.data() + offset,
                    sizeof(progHdr));

        offset += m_header.e_phentsize;

        if (progHdr.p_type == PT_LOAD)
        {
            std::cout << std::setw(8) << std::setfill(' ') << std::left;
            std::cout << "LOAD";

            std::cout << "0x";
            std::cout << std::hex << std::setw(8) << std::right << std::setfill('0');
            std::cout << progHdr.p_paddr;
            std::cout << " ";

            std::cout << "0x";
            std::cout << std::hex << std::setw(8) << std::right << std::setfill('0');
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

    std::cout << std::dec;
    std::cout << "Loaded " << loadedSize << " bytes into memory" << std::endl;
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
    std::cout << "  ";
    std::cout << std::setw(4) << std::setfill(' ') << std::left << "Num";
    std::cout << std::setw(18) << std::setfill(' ') << std::left << "Name";
    std::cout << std::setw(18) << std::setfill(' ') << std::left << "Type";
    std::cout << std::setw(11) << std::setfill(' ') << std::left << "Off";
    std::cout << std::endl;

    Elf32_Off offset = m_header.e_shoff;
    for (Elf32_Half i = 0; i < m_header.e_shnum; ++i)
    {
        std::cout << "  ";
        std::cout << std::setw(4) << std::setfill(' ') << std::left << i;

        Elf32_Shdr sectionHdr;
        std::memcpy(&sectionHdr, m_filebytes.data() + offset,
                    sizeof(sectionHdr));

        offset += m_header.e_shentsize;

        // get name:
        std::stringstream name_ss;
        Elf32_Off nameOffset = shStrTable.sh_offset + sectionHdr.sh_name;
        std::byte c;
        while ((c = m_filebytes[nameOffset++]) != std::byte{'\0'})
        {
            name_ss << static_cast<char>(c);
        }
        std::cout << std::setw(18) << std::setfill(' ') << std::left;
        std::cout << name_ss.str();

        std::cout << std::setw(18) << std::setfill(' ') << std::left;
        switch (sectionHdr.sh_type)
        {
            case SHT_STRTAB:
            {
                if (i == m_header.e_shstrndx)
                {
                    std::cout << "STRTAB (shstrtab)";
                }
                else
                {
                    std::cout << "STRTAB";
                    if (strTableFound)
                    {
                        throw std::runtime_error("multiple string tables in ELF");
                    }

                    strTableFound = true;
                    strTableHdr = sectionHdr;
                }
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
                std::cout << "(other)";
        }

        std::cout << "0x";
        std::cout << std::setw(8) << std::setfill('0') << std::right;
        std::cout << sectionHdr.sh_offset;
        std::cout << std::endl;
    }

    // TODO dynamic sections

    if (symTableFound && strTableFound)
    {
        std::cout << std::endl;
        std::cout << "Found symbols: " << std::endl;
        std::cout << "  ";
        std::cout << std::setw(4) << std::setfill(' ') << std::left << "Num";
        std::cout << std::setw(11) << std::setfill(' ') << std::left << "Value";
        std::cout << std::setw(8) << std::setfill(' ') << std::left << "Type";
        std::cout << std::setw(11) << std::setfill(' ') << std::left << "Bind";
        std::cout << std::setw(11) << std::setfill(' ') << std::left << "Name";
        std::cout << std::endl;

        // read symbol table
        Elf32_Word numSymbols = symTableHdr.sh_size / symTableHdr.sh_entsize;
        Elf32_Off symTableOffset = symTableHdr.sh_offset;
        for (Elf32_Word i = 0; i < numSymbols; ++i)
        {
            std::cout << "  ";
            std::cout << std::setw(4) << std::setfill(' ') << std::left;
            std::cout << std::dec;
            std::cout << i;

            Elf32_Sym symbol;
            std::memcpy(&symbol, m_filebytes.data() + symTableOffset,
                        sizeof(symbol));

            symTableOffset += symTableHdr.sh_entsize;

            // decode symbol to store in simHost
            SimHost::Symbol symbolLoad;

            // symbol value:
            std::cout << "0x";
            std::cout << std::setw(8) << std::setfill('0') << std::right;
            std::cout << std::hex << symbol.st_value;
            std::cout << " ";
            symbolLoad.addr = symbol.st_value;

            std::cout << std::setw(8) << std::setfill(' ') << std::left;
            switch (ELF32_ST_BIND(symbol.st_info))
            {
                case STB_LOCAL:
                    std::cout << "LOCAL";
                    break;
                case STB_GLOBAL:
                    std::cout << "GLOBAL";
                    break;
                case STB_WEAK:
                    std::cout << "WEAK";
                    break;
                default:
                    std::cout << "unknown";
            }

            std::cout << std::setw(11) << std::setfill(' ') << std::left;
            switch (ELF32_ST_TYPE(symbol.st_info))
            {
                case STT_NOTYPE:
                    std::cout << "NOTYPE";
                    symbolLoad.type = SimHost::SymbolType::NOTYPE;
                    break;
                case STT_OBJECT:
                    std::cout << "OBJECT";
                    symbolLoad.type = SimHost::SymbolType::OBJECT;
                    break;
                case STT_FUNC:
                    std::cout << "FUNC";
                    symbolLoad.type = SimHost::SymbolType::FUNC;
                    break;
                case STT_SECTION:
                    std::cout << "SECTION";
                    symbolLoad.type = SimHost::SymbolType::SECTION;
                    break;
                case STT_COMMON:
                    std::cout << "COMMON";
                    symbolLoad.type = SimHost::SymbolType::COMMON;
                    break;
                case STT_TLS:
                    std::cout << "TLS";
                    symbolLoad.type = SimHost::SymbolType::TLS;
                    break;
                default:
                    std::cout << "unknown";
                    symbolLoad.type = SimHost::SymbolType::UNKNOWN;
            }

            // symbol name:
            std::stringstream name_ss;
            Elf32_Off nameOffset = strTableHdr.sh_offset + symbol.st_name;
            std::byte c;
            while ((c = m_filebytes[nameOffset++]) != std::byte{'\0'})
            {
                name_ss << static_cast<char>(c);
            }

            std::cout << std::setw(11) << std::setfill(' ') << std::left;
            std::cout << name_ss.str();

            // insert into sim's symbol table
            if (!name_ss.str().empty())
            {
                simHost.SymbolMap().insert(std::make_pair(name_ss.str(), symbolLoad));
            }

            std::cout << std::endl;
        }

        std::cout << std::dec;
        std::cout << "Loaded " << simHost.SymbolMap().size();
        std::cout << " symbols" << std::endl;
    }
}

} // namespace riscvdb
