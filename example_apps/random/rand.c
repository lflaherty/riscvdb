#include <stdint.h>

#define TEST_RAND_SEED 42
#define TEST_ARRAY_LEN 2048
uint32_t rand_array[TEST_ARRAY_LEN];

// random generator prototypes
void seed(uint32_t s);
uint32_t rand(void);

int main(int argc, char* argv[])
{
  seed(TEST_RAND_SEED);
  for (unsigned i = 0; i < TEST_ARRAY_LEN; ++i)
  {
    rand_array[i] = rand();
  }

  return 0;
}

// Pure C pseudo-random generator from https://stackoverflow.com/a/70960914
static uint32_t
    x=123456789,
    y=362436069,
    z=521288629,
    w=88675123,
    v=886756453;
/* replace defaults with five random seed values in calling program */

uint32_t xorshift(void)
{
    uint32_t t = x^(x>>7);
    x=y; y=z; z=w; w=v;
    v=(v^(v<<6))^(t^(t<<13)); 
    return (y+y+1)*v;
} 

// This is a period 2^32 PNRG ([13,17,5]), to fill the seeds.
// You can reverse all three shift directions, you can swap the 13 and 
// the 5, and you can move the 17 last, and it works equally well.
// Oddly, this takes 9 cycles on my box, compared to 6 for the longer
// period generator above.

uint32_t rand(void)
{
    x ^= (x<<13);
    x ^= (x>>17);
    return x ^= (x<<5);
}

void seed(uint32_t s)
{
    x = s;
    v = rand();
    w = rand();
    z = rand();
    y = rand();
}
