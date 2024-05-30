#ifndef RISCVDB_SIMHOST_H
#define RISCVDB_SIMHOST_H

#include <thread>
#include <atomic>
#include "fileloader.h"
#include "memorymap.h"
#include "riscv_processor.h"

namespace riscvdb
{

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

    int LoadFile(FileLoader& loader);

    SimState GetState() const;
    MemoryMap& Memory();
    RiscvProcessor& Processor();

    void ResetSim();
    void Run(unsigned long numInstructions = 0);
    void Pause();

    void SetVerbose(bool verbose);

private:
    std::atomic<SimState> m_state;

    MemoryMap m_mem;
    RiscvProcessor m_processor;

    // the virtual CPU runs in this thread:
    std::thread m_simRunner;

    // pass numInstructions=0 to run indefinitely
    void runSimWorker(unsigned long numInstructions);

};

} // namespace riscvdb

#endif  // RISCVDB_SIMHOST_H
