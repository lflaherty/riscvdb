#ifndef RISCVDB_COMMANDS_RUN_H
#define RISCVDB_COMMANDS_RUN_H

#include "console.h"
#include <string>
#include "simhost.h"

namespace riscvdb {

class CmdRun : public ConsoleCommand {
public:
    CmdRun(SimHost& simHost);

    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    SimHost& m_simHost;

    void sigint_handler(int signum);
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_RUN_H
