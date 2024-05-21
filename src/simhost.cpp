#include "simhost.h"
#include <iostream>

namespace riscvdb {

SimHost::SimHost()
: m_state(IDLE) {
    std::cout << "SimHost running" << std::endl;
}

SimHost::SimState SimHost::GetState() const
{
    return m_state;
}

} // namespace riscvdb
