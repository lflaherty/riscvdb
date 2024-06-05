#ifndef RISCVDB_COMMANDS_PRINT_H
#define RISCVDB_COMMANDS_PRINT_H

#include "console.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include "simhost.h"
#include "memorymap.h"
#include "riscv_processor.h"

namespace riscvdb {

class CmdPrint : public ConsoleCommand {
public:
    CmdPrint(SimHost& simHost);
    ConsoleCommand::CmdRetType run(std::vector<std::string>& args);
    std::string nameLong();
    std::string nameShort();
    std::string helpStr();

private:
    SimHost& m_simHost;

    static const std::string MSG_USAGE;

    enum PrintMode
    {
        MODE_REGISTER_PC,
        MODE_REGISTER_STANDARD,
        MODE_MEM,
        MODE_MEM_SYMBOL,
        MODE_UNKNOWN,
    };

    enum PrintFormat
    {
        PRINT_RAW,
        PRINT_STRING,
        PRINT_INT8,
        PRINT_UINT8,
        PRINT_INT16,
        PRINT_UINT16,
        PRINT_INT32,
        PRINT_UINT32,
        PRINT_INT64,
        PRINT_UINT64,
    };
    static const std::vector<std::string> SUPPORTED_TYPE_STRINGS;
    static const std::unordered_map<PrintFormat, unsigned int> FORMAT_SIZES;

    struct ArgParse
    {
        bool help;
        PrintMode mode;
        PrintFormat format;
        bool sizeSpecified;
        RiscvProcessor::Register regNum;
        MemoryMap::AddrType memAddr;
        std::string memSymbolName;
        MemoryMap::AddrType memSize;

        ArgParse()
        : help(false),
          mode(MODE_UNKNOWN),
          format(PRINT_RAW),
          sizeSpecified(false),
          regNum(0),
          memAddr(0),
          memSize(0)
        {}

        void parse(std::vector<std::string>& args);
    };

    void runPrintRegPC();
    void runPrintRegStd(const ArgParse& parser);
    void runPrintMem(const ArgParse& parser);

    void runPrintMemSingle(const PrintFormat format, const MemoryMap::AddrType memAddr);
    void runPrintMemBlock(const MemoryMap::AddrType memAddr, const MemoryMap::AddrType memSize);
    void runPrintMemArray(const PrintFormat format, const MemoryMap::AddrType memAddr, const MemoryMap::AddrType memSize);

    template <typename T>
    void svalue(std::ostream& os, T value, const PrintFormat format);
};
template <typename T>
void CmdPrint::svalue(std::ostream& os, T value, const PrintFormat format)
{
    switch (format)
    {
    case PRINT_RAW:
        os << static_cast<unsigned int>(value);
        break;
    case PRINT_STRING:
        os << static_cast<char>(value);
        break;
    case PRINT_INT8:
    {
        // we cast to int rather than int8_t because cout will format this as a char
        int8_t i8val = static_cast<int8_t>(value & 0xff);
        os << static_cast<int>(i8val);
        break;
    }
    case PRINT_UINT8:
    {
        uint8_t ui8val = static_cast<int8_t>(value & 0xff);
        os << static_cast<unsigned int>(ui8val);
        break;
    }
    case PRINT_INT16:
        os << static_cast<int16_t>(value & 0xffff);
        break;
    case PRINT_UINT16:
        os << static_cast<uint16_t>(value & 0xffff);
        break;
    case PRINT_INT32:
        os << static_cast<int32_t>(value & 0xffffffff);
        break;
    case PRINT_UINT32:
        os << static_cast<uint32_t>(value & 0xffffffff);
        break;
    case PRINT_INT64:
        os << static_cast<int64_t>(value);
        break;
    case PRINT_UINT64:
        os << static_cast<uint64_t>(value);
        break;
    default:
        break;
    }
}

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_PRINT_H
