#include "commands/quit.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdQuit::run(std::vector<std::string>& args) {
  if (args.size() > 1) {
    std::cerr << "error: unexpected number of args" << std::endl;
    return CmdRetType_ERROR;
  }

  return CmdRetType_QUIT;
}

std::string CmdQuit::nameLong() { return "quit"; }

std::string CmdQuit::nameShort() { return "q"; }

std::string CmdQuit::helpStr() { return "Exit simulator"; }

} // namespace riscvdb
