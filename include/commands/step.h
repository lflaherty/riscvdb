#ifndef RISCVDB_COMMANDS_STEP_H
#define RISCVDB_COMMANDS_STEP_H

#include "console.h"
#include <string>

namespace riscvdb {

class CmdStep : public ConsoleCommand {
public:
    CmdStep(SimHost& simHost);

    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    SimHost& m_simHost;

    void sigint_handler(int signum);

    static const std::string MSG_USAGE;
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_STEP_H
