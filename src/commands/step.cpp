#include "commands/step.h"

#include <iostream>
#include <csignal>
#include <cstring>

namespace riscvdb {

const std::string CmdStep::MSG_USAGE =
"Usage: step number_of_instructions\n"
"Set number of instructions to 0 or omit to run indefinitely";

CmdStep::CmdStep(SimHost& simHost)
: m_simHost(simHost)
{
  // Empty
}

void CmdStep::sigint_handler(int signum)
{
  if (signum == SIGINT)
  {
    if (m_simHost.GetState() == SimHost::RUNNING)
    {
      // user hit Ctrl-C before program finished
      std::cout << std::endl;
      std::cout << "user interrupted" << std::endl;

      // this will eventually change the state
      m_simHost.Pause();
    }
  }
}

ConsoleCommand::CmdRetType CmdStep::run(std::vector<std::string>& args)
{
  unsigned long numInstructions = 1;

  switch(args.size())
  {
    case 1:
      // default - run 1 instruction
      break;

    case 2:
      if (args[1] == "-h" || args[1] == "--help")
      {
        std::cerr << MSG_USAGE << std::endl;
        return CmdRetType_OK;
      }

      try
      {
        numInstructions = std::stoul(args[1]);
        std::cout << "Running " << numInstructions << " instruction(s)" << std::endl;
      }
      catch (std::exception& e)
      {
        std::cerr << "invalid argument " << args[1] << ": " << e.what() << std::endl;
        return CmdRetType_ERROR;
      }
      break;

    default:
      std::cerr << MSG_USAGE << std::endl;
      return CmdRetType_ERROR;
  }

  // if program is not running, quit
  if (m_simHost.GetState() != SimHost::PAUSED)
  {
    std::cerr << "nothing currently running" << std::endl;
    return CmdRetType_ERROR;
  }

  // begin running
  try
  {
    m_simHost.Run(numInstructions);
  }
  catch (std::runtime_error& e)
  {
    std::cerr << e.what() << std::endl;
    return CmdRetType_ERROR;
  }

  // Register a SIGINT handler for this class
  SigIntHandler sigint(std::bind(&CmdStep::sigint_handler, this, std::placeholders::_1));

  // and wait while running
  while (m_simHost.GetState() == SimHost::RUNNING)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  if (m_simHost.GetState() == SimHost::PAUSED)
  {
    std::cout << "target paused" << std::endl;
    std::cout << "PC = 0x";
    std::cout << std::hex << m_simHost.Processor().GetPC() << std::endl;
    std::cout << std::dec << m_simHost.Processor().GetInstructionCount();
    std::cout << " instructions executed" << std::endl;
  }
  else if (m_simHost.GetState() == SimHost::TERMINATED)
  {
    std::cout << "target terminated" << std::endl;
    std::cout << "PC = 0x";
    std::cout << std::hex << m_simHost.Processor().GetPC() << std::endl;
    std::cout << std::dec << m_simHost.Processor().GetInstructionCount();
    std::cout << " instructions executed" << std::endl;
  }

  return CmdRetType_OK;
}

std::string CmdStep::nameLong() { return "step"; }

std::string CmdStep::nameShort() { return "s"; }

std::string CmdStep::helpStr() { return "Single step instructions"; }

} // namespace riscvdb
