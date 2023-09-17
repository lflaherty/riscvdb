#include "commands/help.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdHelp::run() {
    // This shouldn't be executed directly
    return CmdRetType_ERROR;
}

std::string CmdHelp::nameLong() { return "help"; }

std::string CmdHelp::nameShort() { return "h"; }

std::string CmdHelp::helpStr() { return "Display this help page."; }

} // namespace riscvdb
