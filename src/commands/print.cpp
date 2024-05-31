#include "commands/print.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace riscvdb {

const std::string CmdPrint::MSG_USAGE =
"usage: print item_to_print [size]\n"
"item_to_print can be:\n"
"\ta register        denoted by x0..x31 or PC\n"
"\ta memory address  denoted by a 0x#### value\n"
"e.g.: 'print x5' displays the x5 register.\n"
"      'print 0x7F000' displays the contents of memory at 0x7F000\n"
"\n"
"size (optional) can be used to determine how much memory should be displayed\n"
"note that this parameter is unused when printing registers\n"
"By default, memory addresses will be printed as a 32-bit word\n"
"When specifying a size, an array of bytes of that size will be printed";

CmdPrint::CmdPrint(SimHost& simHost)
: m_simHost(simHost)
{
  // empty
}

ConsoleCommand::CmdRetType CmdPrint::run(std::vector<std::string>& args) {
  ArgParse parser;
  try
  {
    parser.parse(args);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << "use print --help or p -h to display usage help" << std::endl;
    return CmdRetType_ERROR;
  }

  if (parser.help)
  {
    std::cout << MSG_USAGE << std::endl;
    return CmdRetType_OK;
  }

  switch (parser.mode)
  {
    case MODE_REGISTER_PC:
      if (parser.sizeSpecified)
      {
        std::cerr << "not expecting size parameter when printing PC" << std::endl;
        return CmdRetType_ERROR;
      }
      runPrintRegPC();
      break;

    case MODE_REGISTER_STANDARD:
      if (parser.sizeSpecified)
      {
        std::cerr << "not expecting size parameter when printing register" << std::endl;
        return CmdRetType_ERROR;
      }
      runPrintRegStd(parser);
      break;

    case MODE_MEM:
      try
      {
        runPrintMem(parser);
      }
      catch (std::exception& e)
      {
        std::cerr << e.what() << std::endl;
        return CmdRetType_ERROR;
      }
      break;

    default:
      std::cerr << "invalid parameters" << std::endl;
      return CmdRetType_ERROR;
  }

  return CmdRetType_OK;
}

std::string CmdPrint::nameLong() { return "print"; }

std::string CmdPrint::nameShort() { return "p"; }

std::string CmdPrint::helpStr() { return "Print register value"; }

void CmdPrint::ArgParse::parse(std::vector<std::string>& args)
{
  unsigned int argCounter = 0;

  if (args.size() <= 1 || args.size() > 3)
  {
    throw std::invalid_argument("expecting 1 or 2 arguments");
  }

  for (const std::string& arg : args)
  {
    // skip first arg
    if (argCounter == 0)
    {
      argCounter++;
      continue;
    }

    // help requested?
    if (arg == "-h" || arg == "--help")
    {
      help = true;
      break;
    }

    // main print request
    if (argCounter == 1)
    {
      // main print request is pc?
      if (arg == "pc" || arg == "PC")
      {
        mode = MODE_REGISTER_PC;
        argCounter++;
        continue;
      }
      // main print request is general register?
      else if (arg.size() >= 2 && arg[0] == 'x')
      {
        // maybe a register
        std::string substr = arg.substr(1, arg.size() - 1);

        regNum = std::stoul(substr, nullptr, 10);
        if (regNum > 31)
        {
          throw std::out_of_range("register number exceeds maximum of 31");
        }

        mode = MODE_REGISTER_STANDARD;
        argCounter++;
        continue;
      }
      // main print request is memory address?
      else if (arg.size() >= 3 && arg[0] == '0' && std::tolower(arg[1]) == 'x')
      {
        // maybe a memory location
        std::string substr = arg.substr(2, arg.size() - 2);
        memAddr = std::stoull(substr, nullptr, 16); // input is in hex

        mode = MODE_MEM;
        argCounter++;
        continue;
      }
      else
      {
        std::stringstream ss;
        ss << "unknown argument " << arg;
        throw std::invalid_argument(ss.str());
      }
    }
    else if (argCounter == 2)
    {
      if (arg.size() >= 3 && arg[0] == '0' && std::tolower(arg[1]) == 'x')
      {
        // maybe a memory location
        std::string substr = arg.substr(2, arg.size() - 2);
        memSize = std::stoull(substr, nullptr, 16); // input is in hex
        sizeSpecified = true;

        argCounter++;
        continue;
      }
      else
      {
        throw std::invalid_argument("expecting size as hex number");
      }
    }
  }
}

void CmdPrint::runPrintRegPC()
{
  std::cout << "PC = 0x";
  std::cout << std::hex << std::setfill('0') << std::setw(8);
  std::cout << m_simHost.Processor().GetPC();
  std::cout << std::endl;
}

void CmdPrint::runPrintRegStd(const CmdPrint::ArgParse& parser)
{
  std::cout << std::dec << "x" << parser.regNum << " = 0x";
  std::cout << std::hex << std::setfill('0') << std::setw(8);
  std::cout << m_simHost.Processor().GetReg(parser.regNum);
  std::cout << std::endl;
}

void CmdPrint::runPrintMem(const CmdPrint::ArgParse& parser)
{
  if (parser.sizeSpecified)
  {
    // print array of bytes
    // print rows of 16 bytes (similar to xxd output)
    for (MemoryMap::AddrType i = 0; i < parser.memSize; i += 16)
    {
      // address on left:
      std::cout << std::hex << std::setfill('0') << std::setw(8);
      std::cout << i << ": ";

      MemoryMap::AddrType bytesThisRow = std::min(16ULL, parser.memSize - i);
      for (MemoryMap::AddrType j = 0; j < bytesThisRow; j++)
      {
        std::byte b;
        MemoryMap::AddrType addr = parser.memAddr + i + j;
        m_simHost.Memory().Get(addr, b);

        std::cout << std::hex << std::setfill('0') << std::setw(4);
        std::cout << static_cast<unsigned int>(b);

        if (j < (bytesThisRow - 1))
        {
          std::cout << " ";
        }
      }

      std::cout << std::endl;
    }
  }
  else
  {
    // print single word
    uint32_t w = m_simHost.Memory().ReadWord(parser.memAddr);

    std::cout << std::hex << std::setfill('0') << std::setw(8);
    std::cout << parser.memAddr << ": ";
    std::cout << std::hex << std::setfill('0') << std::setw(8);
    std::cout << w << std::endl;
  }
}

} // namespace riscvdb
