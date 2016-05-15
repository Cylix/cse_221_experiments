#include <iostream>
#include <cmath>

uint64_t
rdtscp(void) {
  uint32_t eax, edx;

  __asm__ __volatile__("rdtscp" //! rdtscp instruction
		       : "+a" (eax), "=d" (edx) //! output
		       : //! input
		       : "%ecx"); //! registers

  return (((uint64_t)edx << 32) | eax);
}

int
main(void) {
  unsigned int size = std::pow(2, 26);

  uint64_t before;
  uint64_t after;

  // 2^26 entries of 8 bytes = 2^28B (500MB)
  uint64_t* array = new uint64_t[size];

  before = rdtscp();
  //! step of 8 (8 * 8 bytes = 64 bytes = cache line size)
  for (unsigned int i = 0; i < size; i += 8) {
    array[i] = 42;
  }
  after = rdtscp();

  std::cout << after - before - 43620761 << " cycles" << std::endl;
  std::cout << (unsigned int)((after - before - 43620761) * 0.4) << " ns: " << std::endl;
  std::cout << "67,108,864 bytes" << std::endl;
  std::cout << 67108864 / ((after - before - 43620761) * 0.4) << " B/ns" << std::endl;
  
  delete[] array;
  
  return 0;
}
