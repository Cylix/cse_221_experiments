#include <iostream>
#include <cmath>

#define NUMBER_ITER 1000000
#define STRIDE 128
#define MIN_POWER 8
#define MAX_POWER 32

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
  for (int power = MIN_POWER; power < MAX_POWER; ++power) {
    unsigned int size = std::pow(2, power);
    char* array = new char[size];

    char **p = (char**)array;
    for (unsigned int i = STRIDE; i < size; i += STRIDE) {
      *p = (char*)(p) + STRIDE;
      p = (char**)*p;
    }
    *p = (char*)array;
    p = (char**)array;

    uint64_t before;
    uint64_t after;
    double sum = 0;

    for (unsigned int retry = 0; retry < 1000; ++retry) {
      before = rdtscp();
      for (unsigned int i = 0; i < NUMBER_ITER; ++i) {
	p = (char**)*p;
      }
      after = rdtscp();
      sum += (after - before - 34 - 5200048) / 1000000.f;
    }
    
    std::cout << power << ": " << sum / 1000.f << std::endl;
    
    delete[] array;
  }
  
  return 0;
}

