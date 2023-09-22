#include "commands/break.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdBreak::run(std::vector<std::string>& args) {
  (void)args;
  std::cerr << "Command not implemented" << std::endl;
  return CmdRetType_ERROR;
}

std::string CmdBreak::nameLong() { return "break"; }

std::string CmdBreak::nameShort() { return "b"; }

std::string CmdBreak::helpStr() { return "Create breakpoint"; }

} // namespace riscvdb
