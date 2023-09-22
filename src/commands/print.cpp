#include "commands/print.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdPrint::run(std::vector<std::string>& args) {
  (void)args;
  std::cerr << "Command not implemented" << std::endl;
  return CmdRetType_ERROR;
}

std::string CmdPrint::nameLong() { return "print"; }

std::string CmdPrint::nameShort() { return "p"; }

std::string CmdPrint::helpStr() { return "Print register value"; }

} // namespace riscvdb
