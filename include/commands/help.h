#ifndef RISCVDB_COMMANDS_HELP_H
#define RISCVDB_COMMANDS_HELP_H

#include "console.h"
#include <vector>
#include <string>
#include <memory>

namespace riscvdb {

class CmdHelp : public ConsoleCommand {
public:
    CmdHelp(Console* console);
    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    Console* m_console;
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_HELP_H
