#include <iostream>

uint64_t
rdtscp(void) {
  uint32_t eax, edx;

  __asm__ __volatile__("rdtscp" //! rdtscp instruction
		       : "+a" (eax), "=d" (edx) //! output
		       : //! input
		       : "%ecx"); //! registers

  return (((uint64_t)edx << 32) | eax);
}

volatile __attribute__ ((noinline)) void fct() {}
volatile __attribute__ ((noinline)) void fct(int) {}
volatile __attribute__ ((noinline)) void fct(int, int) {}
volatile __attribute__ ((noinline)) void fct(int, int, int) {}
volatile __attribute__ ((noinline)) void fct(int, int, int, int) {}
volatile __attribute__ ((noinline)) void fct(int, int, int, int, int) {}
volatile __attribute__ ((noinline)) void fct(int, int, int, int, int, int) {}
volatile __attribute__ ((noinline)) void fct(int, int, int, int, int, int, int) {}

int main(void) {
  uint64_t before;
  uint64_t after;

  before = rdtscp();
  for (unsigned int i = 0; i < 10000; ++i)
    fct();
  after = rdtscp();
  std::cout << (after - before - 52048.167 - 34) / 10000.f << std::endl;

  before = rdtscp();
  for (unsigned int i = 0; i < 10000; ++i)
    fct(i);
  after = rdtscp();
  std::cout << (after - before - 52048.167 - 34) / 10000.f << std::endl;

  before = rdtscp();
  for (unsigned int i = 0; i < 10000; ++i)
    fct(i, i);
  after = rdtscp();
  std::cout << (after - before - 52048.167 - 34) / 10000.f << std::endl;

  before = rdtscp();
  for (unsigned int i = 0; i < 10000; ++i)
    fct(i, i, i);
  after = rdtscp();
  std::cout << (after - before - 52048.167 - 34) / 10000.f << std::endl;

  before = rdtscp();
  for (unsigned int i = 0; i < 10000; ++i)
    fct(i, i, i, i);
  after = rdtscp();
  std::cout << (after - before - 52048.167 - 34) / 10000.f << std::endl;

  before = rdtscp();
  for (unsigned int i = 0; i < 10000; ++i)
    fct(i, i, i, i, i);
  after = rdtscp();
  std::cout << (after - before - 52048.167 - 34) / 10000.f << std::endl;

  before = rdtscp();
  for (unsigned int i = 0; i < 10000; ++i)
    fct(i, i, i, i, i, i);
  after = rdtscp();
  std::cout << (after - before - 52048.167 - 34) / 10000.f << std::endl;

  before = rdtscp();
  for (unsigned int i = 0; i < 10000; ++i)
    fct(i, i, i, i, i, i, i);
  after = rdtscp();
  std::cout << (after - before - 52048.167 - 34) / 10000.f << std::endl;

  return 0;
}
