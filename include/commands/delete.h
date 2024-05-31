#ifndef RISCVDB_COMMANDS_DELETE_H
#define RISCVDB_COMMANDS_DELETE_H

#include "console.h"
#include <string>
#include "simhost.h"

namespace riscvdb {

class CmdDelete : public ConsoleCommand {
public:
    CmdDelete(SimHost& simHost);

    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    SimHost& m_simHost;

    static const std::string MSG_USAGE;
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_DELETE_H
