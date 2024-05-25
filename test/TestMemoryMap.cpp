#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "memorymap.h"

namespace rv = riscvdb;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    rv::MemoryMap memoryMap(0, 1024 * 1024 * 1024); // start at 0, 1 GiB

    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned char> distribution(0, 255);

    std::vector<std::byte> testData(100 * 1024 * 1024);
    for (std::byte& b : testData)
    {
        b = static_cast<std::byte>(distribution(generator));
    }
    std::cout << "generated data" << std::endl;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    memoryMap.Put(0x0100, testData);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time elapsed = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl;


    std::cout << "Checking data" << std::endl;
    std::vector<std::byte> dataCheck(testData.size());
    for (std::size_t i = 0x100; i < testData.size(); ++i)
    {
        memoryMap.Get(i, dataCheck[i]);
        if (dataCheck[i] != testData[i])
        {
            std::cout << "!! address " << i;
            std::cout << " got ";
            std::cout << static_cast<unsigned char>(dataCheck[i]);
            std::cout << " expecting ";
            std::cout << static_cast<unsigned char>(testData[i]);
            std::cout << std::endl;
        }
    }

    return 0;
}
