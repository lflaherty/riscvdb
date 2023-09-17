#include "commands/quit.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdQuit::run() {
  return CmdRetType_QUIT;
}

std::string CmdQuit::nameLong() { return "quit"; }

std::string CmdQuit::nameShort() { return "q"; }

std::string CmdQuit::helpStr() { return "Exit simulator"; }

} // namespace riscvdb
