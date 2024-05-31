#ifndef RISCVDB_SIMHOST_H
#define RISCVDB_SIMHOST_H

#include <thread>
#include <atomic>
#include <unordered_map>
#include "fileloader.h"
#include "memorymap.h"
#include "riscv_processor.h"

namespace riscvdb
{

// forward declaration
class FileLoader;

class SimHost {
public:
    static const MemoryMap::AddrType DEFAULT_MEM_ORIGIN = 0x0;
    static const MemoryMap::AddrType DEFAULT_MEM_SIZE = 0x100000000ULL; // 4 GiB

    enum SimState {
        IDLE,
        RUNNING,
        PAUSED,
        TERMINATED,
    };

    SimHost();
    ~SimHost();

    int LoadFile(const std::string& path);
    int LoadFile(FileLoader& loader);

    SimState GetState() const;
    MemoryMap& Memory();
    RiscvProcessor& Processor();

    void ResetSim();
    void Run(unsigned long numInstructions = 0);
    void Pause();

    // returns breakpoint number
    int AddBreakpoint(const MemoryMap::AddrType address);
    void RemoveBreakpoint(const unsigned int breakpointNumber);
    void ClearBreakpoints();

    enum SymbolType
    {
        NOTYPE,
        OBJECT,
        FUNC,
        SECTION,
        COMMON,
        TLS,
        UNKNOWN,
    };
    struct Symbol
    {
        SymbolType type;
        MemoryMap::AddrType addr;
    };
    typedef std::unordered_map<std::string,Symbol> SymbolMapType;
    SymbolMapType& SymbolMap();

    void SetVerbose(bool verbose);

private:
    std::string m_loadedBin;

    std::atomic<SimState> m_state;

    MemoryMap m_mem;
    RiscvProcessor m_processor;

    // maps Addr -> breakpoint number to have good run-time efficiency (for
    // O(1)  lookup time for searching for instructions when running)
    std::unordered_map<MemoryMap::AddrType, unsigned int> m_breakpoints;
    unsigned int m_breakpointCount;

    SymbolMapType m_symbolMap;

    // the virtual CPU runs in this thread:
    std::thread m_simRunner;

    // pass numInstructions=0 to run indefinitely
    void runSimWorker(unsigned long numInstructions);

};

} // namespace riscvdb

#endif  // RISCVDB_SIMHOST_H
