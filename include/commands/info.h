#ifndef RISCVDB_COMMANDS_INFO_H
#define RISCVDB_COMMANDS_INFO_H

#include "console.h"
#include "simhost.h"
#include "riscv_processor.h"

namespace riscvdb {

class CmdInfo : public ConsoleCommand {
public:
    CmdInfo(SimHost& simHost);
    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    SimHost& m_simHost;

    static const std::string MSG_USAGE;
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_INFO_H
