#include "commands/load.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdLoad::run(std::vector<std::string>& args) {
  (void)args;
  std::cerr << "Command not implemented" << std::endl;
  return CmdRetType_ERROR;
}

std::string CmdLoad::nameLong() { return "load"; }

std::string CmdLoad::nameShort() { return "l"; }

std::string CmdLoad::helpStr() { return "Load a binary"; }

} // namespace riscvdb
