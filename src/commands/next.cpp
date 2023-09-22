#include "commands/next.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdNext::run(std::vector<std::string>& args) {
  (void)args;
  std::cerr << "Command not implemented" << std::endl;
  return CmdRetType_ERROR;
}

std::string CmdNext::nameLong() { return "next"; }

std::string CmdNext::nameShort() { return "n"; }

std::string CmdNext::helpStr() { return "Run next instruction and break"; }

} // namespace riscvdb
