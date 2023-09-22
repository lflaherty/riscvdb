#ifndef RISCVDB_COMMANDS_CONTINUE_H
#define RISCVDB_COMMANDS_CONTINUE_H

#include "console.h"
#include <string>

namespace riscvdb {

class CmdContinue : public ConsoleCommand {
public:
    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_CONTINUE_H
