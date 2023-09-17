#include "commands/help.h"

#include <iostream>

namespace riscvdb {

CmdHelp::CmdHelp(Console* console) : m_console(console) {}

ConsoleCommand::CmdRetType CmdHelp::run(std::vector<std::string>& args) {
    switch (args.size()) {
        case 1:
            m_console->printHelp();
            break;
        case 2:
            m_console->printHelpCmd(args[1]);
            break;
        default:
            std::cerr << "error: unexpected parameters" << std::endl;
            return CmdRetType_ERROR;
    }

    return CmdRetType_OK;
}

std::string CmdHelp::nameLong() { return "help"; }

std::string CmdHelp::nameShort() { return "h"; }

std::string CmdHelp::helpStr() { return "Display this help page."; }

} // namespace riscvdb
