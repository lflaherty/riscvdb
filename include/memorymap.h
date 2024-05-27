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
    typedef unsigned long long AddrType;

    MemoryMap(const AddrType memAddrStart, const AddrType memSize);

    std::size_t BlockSize() const;

    void Put(const AddrType address, const std::byte& data);
    void Put(const AddrType address, const std::vector<std::byte>& data);
    void Get(const AddrType address, std::byte& data_out);

private:
    const AddrType m_addrLower;
    const AddrType m_addrUpper;
    const AddrType m_memSize;

    typedef std::array<std::byte, DEFAULT_BLOCK_SIZE> MemBlockType;
    std::unordered_map<AddrType, std::unique_ptr<MemBlockType>> m_mem;
};

} // namespace riscvdb

#endif  // RISCVDB_MEMORYMAP_H
