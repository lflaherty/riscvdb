#ifndef RISCVDB_COMMANDS_LOAD_H
#define RISCVDB_COMMANDS_LOAD_H

#include "console.h"
#include <string>

namespace riscvdb {

class CmdLoad : public ConsoleCommand {
public:
    CmdLoad(SimHost& simHost);
    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    SimHost& m_simHost;

    static const std::string MSG_USAGE;
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_LOAD_H
