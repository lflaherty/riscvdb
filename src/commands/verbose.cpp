#include "commands/verbose.h"

#include <iostream>
#include <csignal>

namespace riscvdb {

const std::string CmdVerbose::MSG_USAGE =
"usage: verbose [option]\n"
"\t0,off,false\tsilence verbose output\n"
"\t1,on,true\tenable verbose output";

CmdVerbose::CmdVerbose(SimHost& simHost)
: m_simHost(simHost)
{
  // Empty
}

ConsoleCommand::CmdRetType CmdVerbose::run(std::vector<std::string>& args)
{
  if (args.size() != 2)
  {
    std::cerr << MSG_USAGE << std::endl;
    return CmdRetType_ERROR;
  }

  std::string& opt = args[1];
  if (opt == "1" || opt == "on" || opt == "true")
  {
    m_simHost.SetVerbose(true);
  }
  else if (opt == "0" || opt == "off" || opt == "false")
  {
    m_simHost.SetVerbose(false);
  }
  else
  {
    std::cerr << MSG_USAGE << std::endl;
    return CmdRetType_ERROR;
  }

  return CmdRetType_OK;
}

std::string CmdVerbose::nameLong() { return "verbose"; }

std::string CmdVerbose::nameShort() { return "v"; }

std::string CmdVerbose::helpStr() { return "Start execution from the beginning"; }

} // namespace riscvdb
