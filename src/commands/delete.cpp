#include "commands/delete.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdDelete::run(std::vector<std::string>& args) {
  (void)args;
  std::cerr << "Command not implemented" << std::endl;
  return CmdRetType_ERROR;
}

std::string CmdDelete::nameLong() { return "delete"; }

std::string CmdDelete::nameShort() { return "d"; }

std::string CmdDelete::helpStr() { return "Delete breakpoint"; }

} // namespace riscvdb
