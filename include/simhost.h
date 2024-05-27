#ifndef RISCVDB_SIMHOST_H
#define RISCVDB_SIMHOST_H

#include <thread>
#include <atomic>
#include "fileloader.h"
#include "memorymap.h"

namespace riscvdb
{

class SimHost {
public:
    static const MemoryMap::AddrType DEFAULT_MEM_ORIGIN = 0x0;
    static const MemoryMap::AddrType DEFAULT_MEM_SIZE = 4 * 1024 * 1024; // 4 MiB

    enum SimState {
        IDLE,
        RUNNING,
        PAUSED,
        TERMINATED,
    };

    SimHost();

    int LoadFile(FileLoader& loader);

    SimState GetState() const;
    MemoryMap& Memory();

    void ResetSim();
    void Run();
    void Pause();

private:
    std::atomic<SimState> m_state;

    MemoryMap m_mem;

    // the virtual CPU runs in this thread:
    std::thread m_simRunner;

    void runSimWorker();

};

} // namespace riscvdb

#endif  // RISCVDB_SIMHOST_H
