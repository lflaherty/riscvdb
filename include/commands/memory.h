#ifndef RISCVDB_COMMANDS_MEMORY_H
#define RISCVDB_COMMANDS_MEMORY_H

#include "console.h"
#include <string>
#include "memorymap.h"

namespace riscvdb {

class CmdMemory : public ConsoleCommand {
public:
    CmdMemory(MemoryMap& mem);
    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    MemoryMap& m_mem;
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_MEMORY_H
