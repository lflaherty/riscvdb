#include "simhost.h"
#include <iostream>
#include <thread>
#include <chrono>

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

void SimHost::ResetSim()
{
    // TODO reset processor
    m_state = IDLE;
}

void SimHost::Run()
{
    if (m_state == RUNNING)
    {
        throw std::runtime_error("executable already running");
    }

    m_state = RUNNING;
    m_simRunner = std::thread(&SimHost::runSimWorker, this);
}

void SimHost::Pause()
{
    m_state = PAUSED;
    m_simRunner.join();
}

void SimHost::runSimWorker()
{
    // TODO - temporary worker that runs for 10s
    unsigned int counter = 0;
    while(m_state == RUNNING)
    {
        counter++;
        std::cout << "ping from thread - " << counter << std::endl;

        if (counter == 10)
        {
            m_state = TERMINATED;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

} // namespace riscvdb
