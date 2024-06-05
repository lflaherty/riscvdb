#define FIB_N 20

unsigned int fib_result = 0;

unsigned int fib(unsigned int n) {
  if (n == 0)
    return 0;
  else if (n == 1)
    return 1;
  else
    return (fib(n - 1) + fib(n - 2));
}

int main(void) {
  fib_result = fib(FIB_N);
  return 0;
}
