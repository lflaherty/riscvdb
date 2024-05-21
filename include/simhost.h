#ifndef RISCVDB_SIMHOST_H
#define RISCVDB_SIMHOST_H

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

    SimState GetState() const;

private:
    SimState m_state;
};

} // namespace riscvdb

#endif  // RISCVDB_SIMHOST_H
