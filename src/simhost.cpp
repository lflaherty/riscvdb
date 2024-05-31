#include "simhost.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <utility>

namespace riscvdb {

SimHost::SimHost()
: m_state(IDLE),
  m_mem(DEFAULT_MEM_ORIGIN, DEFAULT_MEM_SIZE),
  m_processor(m_mem),
  m_breakpointCount(0)
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

int SimHost::LoadFile(const std::string& pathStr)
{
    std::filesystem::path path(pathStr);

    if (!std::filesystem::exists(path))
    {
        std::cerr << "File " << pathStr << " does not exist" << std::endl;
        return -1;
    }

    std::cout << "Loading executable " << pathStr << std::endl;

    std::string ext = path.extension();
    try
    {
        if (ext == riscvdb::ElfFileLoader::EXT) {
            riscvdb::ElfFileLoader elfFileLoader(pathStr);
            LoadFile(elfFileLoader);
        } else if (ext == std::string(".bin")) {
            std::cerr << "raw binaries not yet supported" << std::endl;
            return -1;
        } else {
            std::cerr << "unexpected filetype " << ext << std::endl;
            return -1;
        }
    }
    catch (std::runtime_error& err)
    {
        std::cerr << "failed to load file" << std::endl;
        std::cerr << err.what() << std::endl;
        return -1;
    }

    return 0;
}

int SimHost::LoadFile(FileLoader& loader)
{
    // erase memory first...
    // this only supports one binary at a time
    m_mem.Clear();
    // TODO clear symbols

    m_loadedBin = loader.PathStr();  // make a copy for if we need to reload
    loader.LoadMemory(*this);
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
    m_state = IDLE;
    if (m_simRunner.joinable())
    {
        m_simRunner.join();
    }

    m_mem.Clear();
    m_processor.Reset();

    // now that memory is cleared, reload original ELF back in
    std::cout << "reloading binary" << std::endl;
    LoadFile(m_loadedBin);
}

void SimHost::Run(unsigned long numInstructions)
{
    if (m_state == RUNNING)
    {
        throw std::runtime_error("executable already running");
    }

    // if a previous run hasn't terminated the thread, do that
    // (thread only runs on m_state == RUNNING)
    if (m_simRunner.joinable())
    {
        m_simRunner.join();
    }

    m_state = RUNNING;
    m_simRunner = std::thread(&SimHost::runSimWorker, this, numInstructions);
}

void SimHost::Pause()
{
    m_state = PAUSED;
    m_simRunner.join();
}

int SimHost::AddBreakpoint(MemoryMap::AddrType addr)
{
    auto it = m_breakpoints.find(addr);
    if (it != m_breakpoints.end())
    {
        std::stringstream ss;
        ss << "breakpoint at address " << addr << " already exists: ";
        ss << "breakpoint " << it->second;
        throw std::runtime_error(ss.str());
    }

    unsigned int bkptNum = m_breakpointCount + 1;
    m_breakpointCount++;

    m_breakpoints.insert(std::make_pair(addr, bkptNum));

    return bkptNum;
}

void SimHost::RemoveBreakpoint(const unsigned int breakpointNumber)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
                           [&breakpointNumber](auto&& p) { return p.second == breakpointNumber; });

    if (it == std::end(m_breakpoints))
    {
        throw std::invalid_argument("breakpoint number not found");
    }

    m_breakpoints.erase(it);
}

void SimHost::ClearBreakpoints()
{
    m_breakpoints.clear();
    // Note: we don't reset the breakpoint counter
}

void SimHost::SetVerbose(bool verbose)
{
    m_processor.SetVerbose(verbose);
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
            continue;
        }

        // check for illegal instruction interrupt
        uint32_t csr_mcause = m_processor.GetCSRValue(RiscvProcessor::csr_mcause);
        if ((csr_mcause & 0xF) == RiscvProcessor::ex_illegal_instruction.exceptionCode)
        {
            // illegal instruction :(
            std::cout << "illegal instruction at PC = 0x";
            std::cout << std::hex << std::setfill('0') << std::setw(8);
            std::cout << m_processor.GetPC();
            std::cout << std::endl;

            m_state = TERMINATED;
            continue;
        }

        // check for machine breakpoint
        if ((csr_mcause & 0xF) == RiscvProcessor::ex_breakpoint.exceptionCode)
        {
            // illegal instruction :(
            std::cout << "machine breakpoint at PC = 0x";
            std::cout << std::hex << std::setfill('0') << std::setw(8);
            std::cout << m_processor.GetPC();
            std::cout << std::endl;

            m_state = PAUSED;
            continue;
        }

        // check for host breakpoint
        auto currentPC = m_processor.GetPC();
        auto bkpt_it = m_breakpoints.find(currentPC);
        if (bkpt_it != m_breakpoints.end())
        {
            // host breakpoint found
            std::cout << "breakpoint " << bkpt_it->second << " hit" << std::endl;

            m_state = PAUSED;
            continue;
        }
    }
}

} // namespace riscvdb
