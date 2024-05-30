#ifndef RISCVDB_COMMANDS_VERBOSE_H
#define RISCVDB_COMMANDS_VERBOSE_H

#include "console.h"
#include <string>
#include "simhost.h"

namespace riscvdb {

class CmdVerbose : public ConsoleCommand {
public:
    CmdVerbose(SimHost& simHost);

    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    SimHost& m_simHost;

    static const std::string MSG_USAGE;
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_VERBOSE_H
