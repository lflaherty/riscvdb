#ifndef RISCVDB_SIMHOST_H
#define RISCVDB_SIMHOST_H

#include "fileloader.h"

namespace riscvdb
{

class SimHost {
public:
    enum SimState {
        IDLE,
        RUNNING,
        PAUSED,
        TERMINATED,
    };

    SimHost();
    int LoadFile(FileLoader& loader);

    SimState GetState() const;

private:
    SimState m_state;
};

} // namespace riscvdb

#endif  // RISCVDB_SIMHOST_H
