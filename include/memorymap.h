#ifndef RISCVDB_MEMORYMAP_H
#define RISCVDB_MEMORYMAP_H

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>

namespace riscvdb
{

static const unsigned long DEFAULT_BLOCK_SIZE = 1024; // 1 KiB

class MemoryMap {
public:

    MemoryMap(const unsigned long memAddrStart, const unsigned long memSize);

    std::size_t BlockSize() const;

    void Put(const unsigned long address, const std::byte& data);
    void Put(const unsigned long address, const std::vector<std::byte>& data);
    void Get(const unsigned long address, std::byte& data_out);

private:
    const unsigned long m_addrLower;
    const unsigned long m_addrUpper;
    const unsigned long m_memSize;

    typedef std::array<std::byte, DEFAULT_BLOCK_SIZE> MemBlockType;
    std::unordered_map<unsigned long, std::unique_ptr<MemBlockType>> m_mem;
};

} // namespace riscvdb

#endif  // RISCVDB_MEMORYMAP_H
