#include "commands/break.h"

#include <iostream>
#include "memorymap.h"

namespace riscvdb {

const std::string CmdBreak::MSG_USAGE =
"usage: break location\n"
"creates a new breakpoint\n"
"location is either a symbol name or a address (in hex)";

CmdBreak::CmdBreak(SimHost& simHost)
: m_simHost(simHost)
{
  // empty
}

ConsoleCommand::CmdRetType CmdBreak::run(std::vector<std::string>& args) {
  if (args.size() != 2)
  {
    std::cerr << MSG_USAGE << std::endl;
    return CmdRetType_ERROR;
  }

  bool locationFound = false;
  MemoryMap::AddrType breakAddr = 0;

  std::string& location = args[1];
  if (location.size() >= 3 && location[0] == '0' && std::tolower(location[1]) == 'x')
  {
    // maybe a memory location
    try
    {
      std::string substr = location.substr(2, location.size() - 2);
      breakAddr = std::stoull(substr, nullptr, 16); // input is in hex
      locationFound = true;
    }
    catch (std::exception& e)
    {
      std::cerr << "bad memory address: " << location << std::endl;
      return CmdRetType_ERROR;
    }
  }
  else
  {
    // try to find a symbol
    // TODO add when ELF symbol loading is implemented
  }

  if (!locationFound)
  {
    std::cerr << "could not set breakpoint for " << location << std::endl;
    return CmdRetType_ERROR;
  }

  try
  {
    unsigned int bkptNum = m_simHost.AddBreakpoint(breakAddr);
    std::cout << "Breakpoint " << bkptNum << " set at " << location << std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return CmdRetType_ERROR;
  }

  return CmdRetType_OK;
}

std::string CmdBreak::nameLong() { return "break"; }

std::string CmdBreak::nameShort() { return "b"; }

std::string CmdBreak::helpStr() { return "Create breakpoint"; }

} // namespace riscvdb
