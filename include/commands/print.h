#ifndef RISCVDB_COMMANDS_PRINT_H
#define RISCVDB_COMMANDS_PRINT_H

#include "console.h"
#include <string>
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

    struct ArgParse
    {
        bool help;
        PrintMode mode;
        bool sizeSpecified;
        RiscvProcessor::Register regNum;
        MemoryMap::AddrType memAddr;
        std::string memSymbolName;
        MemoryMap::AddrType memSize;

        ArgParse()
        : help(false),
          mode(MODE_UNKNOWN),
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
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_PRINT_H
