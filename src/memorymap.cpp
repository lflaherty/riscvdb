#include "memorymap.h"
#include <sstream>
#include <string>

namespace riscvdb {

MemoryMap::MemoryMap(const AddrType memAddrStart, const AddrType memSize)
: m_addrLower(memAddrStart),
  m_addrUpper(memAddrStart + memSize),
  m_memSize(memSize)
{

}

void MemoryMap::Put(const AddrType address, const std::byte& data)
{
    if (address < m_addrLower || address > m_addrUpper)
    {
        std::stringstream ss;
        ss << "address " << address << " is outside of range ";
        ss << "[" << m_addrLower << ", " << m_addrUpper << "]";
        throw std::out_of_range(ss.str());
    }

    AddrType baseAddress = address / DEFAULT_BLOCK_SIZE;
    AddrType offset = address % DEFAULT_BLOCK_SIZE;

    std::unordered_map<AddrType, std::unique_ptr<MemBlockType>>::iterator it;
    it = m_mem.find(baseAddress);
    if (it == m_mem.end())
    {
        std::unique_ptr<MemBlockType> newBlock = std::make_unique<MemBlockType>();
        m_mem[baseAddress] = std::move(newBlock);
    }

    m_mem[baseAddress].get()->at(offset) = data;
}

void MemoryMap::Put(const AddrType address, const std::vector<std::byte>& data)
{
    if (address < m_addrLower || (address + data.size()) > m_addrUpper)
    {
        std::stringstream ss;
        ss << "address ";
        ss << "[" << address << ", " << address + data.size() << "]";
        ss << " is outside of range ";
        ss << "[" << m_addrLower << ", " << m_addrUpper << "]";
        throw std::out_of_range(ss.str());
    }

    AddrType currentAddr = address;
    AddrType bytesRemaining = data.size();
    while (bytesRemaining > 0)
    {
        AddrType baseAddress = currentAddr / DEFAULT_BLOCK_SIZE;
        AddrType offset = currentAddr % DEFAULT_BLOCK_SIZE;
        AddrType bytesToCopy = std::min(DEFAULT_BLOCK_SIZE - offset, bytesRemaining);

        std::unordered_map<AddrType, std::unique_ptr<MemBlockType>>::iterator it;
        it = m_mem.find(baseAddress);
        if (it == m_mem.end())
        {
            std::unique_ptr<MemBlockType> newBlock = std::make_unique<MemBlockType>();
            m_mem[baseAddress] = std::move(newBlock);
        }

        std::copy(data.begin() + currentAddr,
                  data.begin() + currentAddr + bytesToCopy,
                  m_mem[baseAddress].get()->begin() + offset);

        currentAddr += bytesToCopy;
        bytesRemaining -= bytesToCopy;
    }
}

void MemoryMap::Get(const AddrType address, std::byte& data_out)
{
    if (address < m_addrLower || address > m_addrUpper)
    {
        std::stringstream ss;
        ss << "address " << address << " is outside of range ";
        ss << "[" << m_addrLower << ", " << m_addrUpper << "]";
        throw std::out_of_range(ss.str());
    }

    AddrType baseAddress = address / DEFAULT_BLOCK_SIZE;
    AddrType offset = address % DEFAULT_BLOCK_SIZE;

    std::unordered_map<AddrType, std::unique_ptr<MemBlockType>>::iterator it;
    it = m_mem.find(baseAddress);
    if (it == m_mem.end())
    {
        std::unique_ptr<MemBlockType> newBlock = std::make_unique<MemBlockType>();
        m_mem[baseAddress] = std::move(newBlock);
    }

    data_out = m_mem[baseAddress].get()->at(offset);
}

} // namespace riscvdb
