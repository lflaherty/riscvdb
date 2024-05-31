#include "commands/info.h"

#include <iostream>
#include <iomanip>

namespace riscvdb {

const std::string CmdInfo::MSG_USAGE =
"usage: info\n"
"print current state of all machine registers";

CmdInfo::CmdInfo(SimHost& simHost)
: m_simHost(simHost)
{
  // empty
}

ConsoleCommand::CmdRetType CmdInfo::run(std::vector<std::string>& args) {
  if (args.size() != 1)
  {
    std::cerr << MSG_USAGE << std::endl;
    return CmdRetType_ERROR;
  }

  // PC
  std::cout << "PC = 0x";
  std::cout << std::hex << std::setfill('0') << std::setw(8);
  std::cout << m_simHost.Processor().GetPC();
  std::cout << std::endl;

  // Standard registers
  for (unsigned int regNum = 0; regNum < 32; ++regNum)
  {
    std::cout << std::dec << "x" << regNum << " = 0x";
    std::cout << std::hex << std::setfill('0') << std::setw(8);
    std::cout << m_simHost.Processor().GetReg(regNum);
    std::cout << std::endl;
  }

  return CmdRetType_OK;
}

std::string CmdInfo::nameLong() { return "info"; }

std::string CmdInfo::nameShort() { return "i"; }

std::string CmdInfo::helpStr() { return "Display all machine registers"; }

} // namespace riscvdb
