#ifndef RISCVDB_SIMHOST_H
#define RISCVDB_SIMHOST_H

#include "fileloader.h"
#include "memorymap.h"

namespace riscvdb
{

class SimHost {
public:
    static const unsigned long DEFAULT_MEM_ORIGIN = 0x0;
    static const unsigned long DEFAULT_MEM_SIZE = 4 * 1024 * 1024; // 4 MiB

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

private:
    SimState m_state;

    MemoryMap m_mem;
};

} // namespace riscvdb

#endif  // RISCVDB_SIMHOST_H
