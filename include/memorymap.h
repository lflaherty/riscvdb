#ifndef RISCVDB_MEMORYMAP_H
#define RISCVDB_MEMORYMAP_H

#include <cstddef>
#include <vector>
#include <array>
#include <unordered_map>

namespace riscvdb
{

class MemoryMap {
public:
    static const std::size_t DEFAULT_BLOCK_SIZE = 1 * 1024 * 1024; // 1 MiB

    MemoryMap(const std::size_t blockSize = DEFAULT_BLOCK_SIZE);

    std::size_t BlockSize() const;

    void Put(const unsigned long address, const std::byte& data);
    void Put(const unsigned long address, const std::vector<std::byte>& data);
    void Get(const unsigned long address, std::byte& data_out);
private:
    const std::size_t m_blockSize;

    std::unordered_map<unsigned long, std::byte> m_mem;
    // std::unordered_map<unsigned long, std::array<std::byte> > m_mem;
};

} // namespace riscvdb

#endif  // RISCVDB_MEMORYMAP_H
