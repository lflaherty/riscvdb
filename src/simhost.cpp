#include "simhost.h"
#include <iostream>

namespace riscvdb {

SimHost::SimHost()
: m_state(IDLE),
  m_mem(DEFAULT_MEM_ORIGIN, DEFAULT_MEM_SIZE)
{
    // empty
}

int SimHost::LoadFile(FileLoader& loader)
{
    loader.LoadMemory(m_mem);
    return 0;
}

SimHost::SimState SimHost::GetState() const
{
    return m_state;
}

MemoryMap& SimHost::Memory()
{
    return m_mem;
}

} // namespace riscvdb
