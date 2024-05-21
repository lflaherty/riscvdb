#include "simhost.h"
#include <iostream>

namespace riscvdb {

SimHost::SimHost()
: m_state(IDLE)
{
    // empty
}

int SimHost::LoadFile(FileLoader& loader)
{
    (void)loader;
    return 0;
}

SimHost::SimState SimHost::GetState() const
{
    return m_state;
}

} // namespace riscvdb
