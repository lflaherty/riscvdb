#include "commands/continue.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdContinue::run(std::vector<std::string>& args) {
  (void)args;
  std::cerr << "Command not implemented" << std::endl;
  return CmdRetType_ERROR;
}

std::string CmdContinue::nameLong() { return "continue"; }

std::string CmdContinue::nameShort() { return "c"; }

std::string CmdContinue::helpStr() { return "Resume execution from break"; }

} // namespace riscvdb
