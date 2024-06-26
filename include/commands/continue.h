#ifndef RISCVDB_COMMANDS_CONTINUE_H
#define RISCVDB_COMMANDS_CONTINUE_H

#include "console.h"
#include <string>

namespace riscvdb {

class CmdContinue : public ConsoleCommand {
public:
    CmdContinue(SimHost& simHost);

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

#endif // RISCVDB_COMMANDS_CONTINUE_H
