#include "commands/run.h"

#include <iostream>
#include <csignal>
#include <cstring>

namespace riscvdb {

const std::string CmdRun::MSG_USAGE =
"Usage: run [number of instructions]\n"
"Set number of instructions to 0 or omit to run indefinitely";

CmdRun::CmdRun(SimHost& simHost)
: m_simHost(simHost)
{
  // Empty
}

void CmdRun::sigint_handler(int signum)
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

ConsoleCommand::CmdRetType CmdRun::run(std::vector<std::string>& args)
{
  unsigned long numInstructions = 0;

  switch(args.size())
  {
    case 1:
      // default - run indefinitely
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

  // if program is already running, then ask if we want to restart
  if (m_simHost.GetState() == SimHost::PAUSED)
  {
    char select = 0;
    while (!(std::tolower(select) == 'y' || std::tolower(select) == 'n'))
    {
      // TODO: if you ctrl C here, it closes the console...
      std::cout << "Target is already running. Would you like to restart? (y/n) ";
      std::cin >> select;
    }

    if (std::tolower(select) == 'n')
    {
      // ok, just bail then
      return CmdRetType_OK;
    }
    else
    {
      std::cout << "resetting machine" << std::endl;
      // restart target and run
      m_simHost.ResetSim();
    }
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
  SigIntHandler sigint(std::bind(&CmdRun::sigint_handler, this, std::placeholders::_1));

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

std::string CmdRun::nameLong() { return "run"; }

std::string CmdRun::nameShort() { return "r"; }

std::string CmdRun::helpStr() { return "Start execution from the beginning"; }

} // namespace riscvdb
