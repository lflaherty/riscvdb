#include "commands/load.h"

#include <iostream>

namespace riscvdb {

const std::string CmdLoad::MSG_USAGE =
"usage: load filename\n"
"loads binary located at 'filename'.\n"
"only one binary can be loaded at a time.";

CmdLoad::CmdLoad(SimHost& simHost)
: m_simHost(simHost)
{
  // empty
}

ConsoleCommand::CmdRetType CmdLoad::run(std::vector<std::string>& args) {
  if (args.size() != 2)
  {
    std::cerr << MSG_USAGE << std::endl;
    return CmdRetType_ERROR;
  }

  if (args[1] == "--help" || args[1] == "-h")
  {
    std::cout << MSG_USAGE << std::endl;
    return CmdRetType_OK;
  }

  std::string& filename = args[1];
  try
  {
    m_simHost.LoadFile(filename);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return CmdRetType_ERROR;
  }

  return CmdRetType_OK;;
}

std::string CmdLoad::nameLong() { return "load"; }

std::string CmdLoad::nameShort() { return "l"; }

std::string CmdLoad::helpStr() { return "Load a binary"; }

} // namespace riscvdb
