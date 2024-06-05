#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define DATATYPE uint32_t

#define TEST_RAND_SEED 42
#define TEST_ARRAY_LEN 2048
DATATYPE rand_array[TEST_ARRAY_LEN];


int cmpfunc(const void* a, const void* b)
{
   return *(DATATYPE*)a - *(DATATYPE*)b;
}

int main(int argc, char* argv[])
{
    srand(TEST_RAND_SEED);
    for (unsigned i = 0; i < TEST_ARRAY_LEN; ++i)
    {
        rand_array[i] = (DATATYPE)rand();
    }

    qsort(rand_array, TEST_ARRAY_LEN, sizeof(DATATYPE), cmpfunc);

    return 0;
}
