#include "memorymap.h"

namespace riscvdb {

MemoryMap::MemoryMap(const std::size_t blockSize)
: m_blockSize(blockSize)
{

}

std::size_t MemoryMap::BlockSize() const
{
    return m_blockSize;
}

void MemoryMap::Put(const unsigned long address, const std::byte& data)
{
    m_mem[address] = data;
}

void MemoryMap::Put(const unsigned long address, const std::vector<std::byte>& data)
{
    unsigned long addrCount = address;
    for (const std::byte& b : data)
    {
        Put(addrCount, b);
        addrCount++;
    }

    
}

void MemoryMap::Get(const unsigned long address, std::byte& data_out)
{
    data_out = m_mem[address];
}

} // namespace riscvdb
