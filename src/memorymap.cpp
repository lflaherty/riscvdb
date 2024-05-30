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

    AddrType i = 0;  // indexes `address`
    AddrType currentAddr = address;  // raw physical address to be storing in
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

        std::copy(data.begin() + i,
                  data.begin() + i + bytesToCopy,
                  m_mem[baseAddress].get()->begin() + offset);

        i += bytesToCopy;
        currentAddr += bytesToCopy;
        bytesRemaining -= bytesToCopy;
    }
}

void MemoryMap::Get(const AddrType address, std::byte& data_out)
{
    if (address < m_addrLower || address > m_addrUpper)
    {
        std::stringstream ss;
        ss << std::hex;
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

uint32_t MemoryMap::ReadWord(const AddrType address)
{
    // the MemoryMap is byte addressed
    std::array<std::byte, 4> word;

    Get(address + 0, word.at(0));
    Get(address + 1, word.at(1));
    Get(address + 2, word.at(2));
    Get(address + 3, word.at(3));

    uint32_t ret = 0;
    ret |= static_cast<uint32_t>(word[0]) << 0;
    ret |= static_cast<uint32_t>(word[1]) << 8;
    ret |= static_cast<uint32_t>(word[2]) << 16;
    ret |= static_cast<uint32_t>(word[3]) << 24;

    return ret;
}

void MemoryMap::WriteWord(const AddrType address, uint32_t data, uint32_t mask)
{
    uint32_t original = ReadWord(address);
    uint32_t new_val = (original & ~mask) | (data & mask);

    std::vector<std::byte> encoding(4);
    encoding[0] = static_cast<std::byte>((new_val >> 0) & 0xFF);
    encoding[1] = static_cast<std::byte>((new_val >> 8) & 0xFF);
    encoding[2] = static_cast<std::byte>((new_val >> 16) & 0xFF);
    encoding[3] = static_cast<std::byte>((new_val >> 24) & 0xFF);

    Put(address, encoding);
}

void MemoryMap::Clear()
{
    m_mem.clear();
}

} // namespace riscvdb
