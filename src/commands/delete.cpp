#include "commands/delete.h"

#include <iostream>
#include "memorymap.h"

namespace riscvdb {

const std::string CmdDelete::MSG_USAGE =
"usage: delete [breakpoint number]\n"
"deletes a breakpoint\n"
"specify a breakpoint number to delete that one, or omit to delete all breakpoints";

CmdDelete::CmdDelete(SimHost& simHost)
: m_simHost(simHost)
{
  // empty
}

ConsoleCommand::CmdRetType CmdDelete::run(std::vector<std::string>& args) {
  switch (args.size())
  {
    case 1:
      // delete all
      m_simHost.ClearBreakpoints();
      break;

    case 2:
      try
      {
        unsigned int bkptNum = std::stoul(args[1]);
        m_simHost.RemoveBreakpoint(bkptNum);
      }
      catch (std::exception& e)
      {
        std::cerr << "could not delete breakpoint " << args[1] << ": ";
        std::cerr << e.what() << std::endl;
        return CmdRetType_ERROR;
      }
  }

  return CmdRetType_OK;
}

std::string CmdDelete::nameLong() { return "delete"; }

std::string CmdDelete::nameShort() { return "d"; }

std::string CmdDelete::helpStr() { return "Delete a breakpoint"; }

} // namespace riscvdb
