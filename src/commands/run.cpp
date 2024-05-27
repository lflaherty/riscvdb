#include "commands/run.h"

#include <iostream>
#include <csignal>

namespace riscvdb {

CmdRun::CmdRun(SimHost& simHost)
: m_simHost(simHost)
{
  // Empty
}

void CmdRun::sigint_handler(int signum)
{
  if (signum == SIGINT)
  {
    // user hit Ctrl-C before program finished
    std::cout << std::endl;
    std::cout << "user interrupted" << std::endl;

    // this will eventually change the state
    m_simHost.Pause();
  }
}

ConsoleCommand::CmdRetType CmdRun::run(std::vector<std::string>& args)
{
  unsigned long numInstructions = 0;

  switch(args.size())
  {
    case 1:
      // default - run indefinitely
      break;

    case 2:
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
      std::cerr << "Usage: run [number of instructions]" << std::endl;
      return CmdRetType_ERROR;
  }

  if (m_simHost.GetState() == SimHost::RUNNING ||
      m_simHost.GetState() == SimHost::PAUSED)
  {
    std::cerr << "target is already running" << std::endl;
    return CmdRetType_ERROR;
  }

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
  SigIntHandler sigint(std::bind(&CmdRun::sigint_handler, this, std::placeholders::_1));

  while (m_simHost.GetState() == SimHost::RUNNING)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  if (m_simHost.GetState() == SimHost::PAUSED)
  {
    std::cout << "target paused" << std::endl;
    std::cout << "PC = 0x";
    std::cout << std::hex << m_simHost.Processor().GetPC() << std::endl;
  }

  return CmdRetType_OK;
}

std::string CmdRun::nameLong() { return "run"; }

std::string CmdRun::nameShort() { return "r"; }

std::string CmdRun::helpStr() { return "Start execution from the beginning"; }

} // namespace riscvdb
