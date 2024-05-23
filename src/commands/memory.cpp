#include "commands/memory.h"

#include <iostream>
#include <cctype>

namespace riscvdb {

CmdMemory::CmdMemory(MemoryMap& mem)
: m_mem(mem)
{
    // empty
}

ConsoleCommand::CmdRetType CmdMemory::run(std::vector<std::string>& args) {
    switch (args.size())
    {
        case 2:
            MemoryMap::AddrType addr;
            try
            {
                addr = std::stoll(args[1], 0, 16);
            }
            catch (std::exception& e)
            {
                std::cerr << "address " << std::hex << args[1] << " is invalid" << std::endl;
                return CmdRetType_ERROR;
            }

            std::byte b;
            m_mem.Get(addr, b);

            std::cout << std::hex;
            std::cout << "mem[" << addr << "] = 0x";
            std::cout << static_cast<unsigned int>(b);
            std::cout << std::endl;

            break;

        case 3:
            break;

        default:
            std::cerr << "usage:" << std::endl;
            std::cerr << "  memory address\tprints word at location" << std::endl;
            std::cerr << "  memory address size\tprints byte array from location" << std::endl;
            std::cerr << "addresses are in hex" << std::endl;
            break;
    }

    return CmdRetType_OK;
}

std::string CmdMemory::nameLong() { return "memory"; }

std::string CmdMemory::nameShort() { return "m"; }

std::string CmdMemory::helpStr() { return "Get or set memory value"; }

} // namespace riscvdb
