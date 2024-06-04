#include "commands/print.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace riscvdb {

const std::string CmdPrint::MSG_USAGE =
"usage: print item_to_print [size] [print_type]\n"
"item_to_print can be:\n"
"\ta register        denoted by x0..x31 or PC\n"
"\ta memory address  denoted by a 0x#### value\n"
"e.g.: 'print x5' displays the x5 register.\n"
"      'print 0x7F000' displays the contents of memory at 0x7F000\n"
"\n"
"size (optional) can be used to determine how much memory should be displayed\n"
"note that this parameter is unused when printing registers\n"
"By default, memory addresses will be printed as a 32-bit word\n"
"When specifying a size, an array of bytes of that size will be printed\n"
"\n"
"print_type (optional) can be used to change how data is printed.\n"
"options are: raw, string, int8_t, uint8_t, int16_t, uint16_t,\n"
"             int32_t, uint32_t, int64_t, uint64_t.\n"
"default is 'raw', which prints the memory layout.";

const std::vector<std::string> CmdPrint::SUPPORTED_TYPE_STRINGS =
{ "raw", "string", "int8_t", "uint8_t", "int16_t", "uint16_t", "int32_t", "uint32_t", "int64_t", "uint64_t" };

const std::unordered_map<CmdPrint::PrintFormat, unsigned int> CmdPrint::FORMAT_SIZES = {
  // note no "raw", since that isn't used in this way
  { CmdPrint::PRINT_STRING, 1 },
  { CmdPrint::PRINT_INT8, 1 },
  { CmdPrint::PRINT_UINT8, 1 },
  { CmdPrint::PRINT_INT16, 2 },
  { CmdPrint::PRINT_UINT16, 2 },
  { CmdPrint::PRINT_INT32, 4 },
  { CmdPrint::PRINT_UINT32, 4 },
  { CmdPrint::PRINT_INT64, 8 },
  { CmdPrint::PRINT_UINT64, 8 },
};

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
    case MODE_MEM_SYMBOL:
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

  if (args.size() < 2 || args.size() > 4)
  {
    throw std::invalid_argument("incorrect number of parameters");
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
        // try symbol name
        memSymbolName = arg;
        mode = MODE_MEM_SYMBOL;
        argCounter++;
        continue;
      }
    }
    else if (argCounter >= 2)
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
      else if (std::find(SUPPORTED_TYPE_STRINGS.begin(), SUPPORTED_TYPE_STRINGS.end(), arg) != SUPPORTED_TYPE_STRINGS.end())
      {
        // the parameter is a data type
        if (arg == "raw")
        {
          format = PRINT_RAW;
        }
        else if (arg == "string" || arg == "str")
        {
          format = PRINT_STRING;
        }
        else if (arg == "int8_t")
        {
          format = PRINT_INT8;
        }
        else if (arg == "uint8_t")
        {
          format = PRINT_UINT8;
        }
        else if (arg == "int16_t")
        {
          format = PRINT_INT16;
        }
        else if (arg == "uint16_t")
        {
          format = PRINT_UINT16;
        }
        else if (arg == "int32_t")
        {
          format = PRINT_INT32;
        }
        else if (arg == "uint32_t")
        {
          format = PRINT_UINT32;
        }
        else if (arg == "int64_t")
        {
          format = PRINT_INT64;
        }
        else if (arg == "uint64_t")
        {
          format = PRINT_UINT64;
        }
      }
      else
      {
        throw std::invalid_argument("invalid parameter");
      }
    }
  }
}

void CmdPrint::runPrintRegPC()
{
  std::cout << "PC = 0x";
  std::cout << std::hex << std::right <<std::setfill('0') << std::setw(8);
  std::cout << m_simHost.Processor().GetPC();
  std::cout << std::endl;
}

void CmdPrint::runPrintRegStd(const CmdPrint::ArgParse& parser)
{
  std::cout << std::dec << "x" << parser.regNum << " = 0x";
  std::cout << std::hex << std::right <<std::setfill('0') << std::setw(8);
  std::cout << m_simHost.Processor().GetReg(parser.regNum);
  std::cout << std::endl;
}

void CmdPrint::runPrintMem(const CmdPrint::ArgParse& parser)
{
  MemoryMap::AddrType memAddr;

  if (parser.mode == MODE_MEM_SYMBOL)
  {
    // try to find symbol
    auto it = m_simHost.SymbolMap().find(parser.memSymbolName);
    if (it == m_simHost.SymbolMap().end())
    {
      std::stringstream ss;
      ss << "cannot find symbol " << parser.memSymbolName << std::endl;
      throw std::invalid_argument(ss.str());
    }

    SimHost::Symbol& symb = it->second;
    memAddr = symb.addr;
  }
  else
  {
    memAddr = parser.memAddr;
  }

  if (parser.sizeSpecified)
  {
    if (parser.format == PRINT_RAW)
    {
      // print hex dump
      runPrintMemBlock(memAddr, parser.memSize);
    }
    else
    {
      // print array
      runPrintMemArray(parser.format, memAddr, parser.memSize);
    }
  }
  else
  {
    runPrintMemSingle(parser.format, memAddr);
  }
}

void CmdPrint::runPrintMemSingle(const PrintFormat format, const MemoryMap::AddrType memAddr)
{
  // print single word
  uint32_t w_lower = m_simHost.Memory().ReadWord(memAddr);
  uint32_t w_upper = m_simHost.Memory().ReadWord(memAddr + 4);
  uint64_t memData = (static_cast<uint64_t>(w_upper) << 32) |
                      static_cast<uint64_t>(w_lower);

  std::cout << std::hex << std::right <<std::setfill('0') << std::setw(8);
  std::cout << memAddr << ": ";

  // pre-format std::cout
  if (format == PRINT_RAW)
  {
    std::cout << std::hex << std::right << std::setfill('0') << std::setw(8);
  }
  else
  {
    std::cout << std::dec << std::left << std::setw(1);
  }

  svalue(std::cout, memData, format);
  std::cout << std::endl;
}

void CmdPrint::runPrintMemBlock(const MemoryMap::AddrType memAddr, const MemoryMap::AddrType memSize)
{
  // print rows of 16 bytes (similar to xxd output)
  for (MemoryMap::AddrType i = 0; i < memSize; i += 16)
  {
    // address on left:
    std::cout << std::hex << std::right <<std::setfill('0') << std::setw(8);
    std::cout << i << ": ";

    MemoryMap::AddrType bytesThisRow = std::min(16ULL, memSize - i);
    for (MemoryMap::AddrType j = 0; j < bytesThisRow; j++)
    {
      std::byte b;
      MemoryMap::AddrType addr = memAddr + i + j;
      m_simHost.Memory().Get(addr, b);

      std::cout << std::hex << std::right <<std::setfill('0') << std::setw(2);
      std::cout << static_cast<unsigned int>(b);

      if (j % 2 == 1)
      {
        std::cout << " ";
      }
    }

    std::cout << std::endl;
  }
}

void CmdPrint::runPrintMemArray(const CmdPrint::PrintFormat format, const MemoryMap::AddrType memAddr, const MemoryMap::AddrType memSize)
{
  auto it = FORMAT_SIZES.find(format);
  if (it == FORMAT_SIZES.end())
  {
    // this shouldn't happen at all at this point...
    throw std::runtime_error("internal format error");
  }

  unsigned int elementSize = it->second;
  if (memSize % elementSize != 0)
  {
    std::stringstream ss;
    ss << "memory size " << memSize << " is not a multiple of print element size " << elementSize;
    throw std::invalid_argument(ss.str());
  }

  unsigned int numElements = memSize / elementSize;

  if (format == PrintFormat::PRINT_STRING)
  {
    // print characters
    std::cout << std::dec << std::left << std::setw(0);
    for (unsigned int i = 0; i < numElements; ++i)
    {
      std::byte b;
      MemoryMap::AddrType byteAddr = memAddr + i;
      m_simHost.Memory().Get(byteAddr, b);

      if (b == std::byte{'\0'})
      {
        break;
      }

      svalue(std::cout, static_cast<char>(b), format);
    }
    std::cout << std::endl;
  }
  else
  {
    // print array
    std::cout << "{ ";

    for (unsigned int i = 0; i < numElements; ++i)
    {
      // get value byte by byte...
      uint64_t x = 0;
      unsigned int shift = 0;
      for (unsigned int j = 0; j < elementSize; ++j)
      {
        std::byte byte_i;
        MemoryMap::AddrType byteAddr = memAddr + elementSize * i + j;
        m_simHost.Memory().Get(byteAddr, byte_i);

        x |= (static_cast<uint64_t>(byte_i) & 0xff) << shift;
        shift += 8;
      }

      std::cout << std::dec << std::left << std::setw(0);
      svalue(std::cout, x, format);

      if (i < (numElements - 1))
      {
        std::cout << ", ";
      }
    }

    std::cout << " }" << std::endl;
  }
}

} // namespace riscvdb
