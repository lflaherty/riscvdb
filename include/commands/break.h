#ifndef RISCVDB_COMMANDS_BREAK_H
#define RISCVDB_COMMANDS_BREAK_H

#include "console.h"
#include <string>
#include "simhost.h"

namespace riscvdb {

class CmdBreak : public ConsoleCommand {
public:
    CmdBreak(SimHost& simHost);

    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    SimHost& m_simHost;

    static const std::string MSG_USAGE;
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_BREAK_H
