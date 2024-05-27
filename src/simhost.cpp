#include "simhost.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace riscvdb {

SimHost::SimHost()
: m_state(IDLE),
  m_mem(DEFAULT_MEM_ORIGIN, DEFAULT_MEM_SIZE),
  m_processor(m_mem)
{
    // empty
}

SimHost::~SimHost()
{
    if (m_simRunner.joinable())
    {
        m_simRunner.join();
    }
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

RiscvProcessor& SimHost::Processor()
{
    return m_processor;
}

void SimHost::ResetSim()
{
    // TODO reset processor
    m_state = IDLE;
}

void SimHost::Run(unsigned long numInstructions)
{
    if (m_state == RUNNING)
    {
        throw std::runtime_error("executable already running");
    }

    m_state = RUNNING;
    m_simRunner = std::thread(&SimHost::runSimWorker, this, numInstructions);
}

void SimHost::Pause()
{
    m_state = PAUSED;
    m_simRunner.join();
}

void SimHost::runSimWorker(unsigned long numInstructions)
{
    unsigned long instCounter = 0;

    while(m_state == RUNNING)
    {
        instCounter++;
        m_processor.Step();

        if (numInstructions > 0 && instCounter == numInstructions)
        {
            m_state = PAUSED;
        }

        // TODO check for if breakpoint hit
    }
}

} // namespace riscvdb
