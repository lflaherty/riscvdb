#include "commands/run.h"

#include <iostream>

namespace riscvdb {

ConsoleCommand::CmdRetType CmdRun::run(std::vector<std::string>& args) {
  (void)args;
  std::cerr << "Command not implemented" << std::endl;
  return CmdRetType_ERROR;
}

std::string CmdRun::nameLong() { return "run"; }

std::string CmdRun::nameShort() { return "r"; }

std::string CmdRun::helpStr() { return "Start execution from the beginning"; }

} // namespace riscvdb
