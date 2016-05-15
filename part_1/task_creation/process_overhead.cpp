#include <iostream>
#include <unistd.h>

uint64_t
rdtscp(void) {
  uint32_t eax, edx;

  __asm__ __volatile__("rdtscp" //! rdtscp instruction
		       : "+a" (eax), "=d" (edx) //! output
		       : //! input
		       : "%ecx"); //! registers

  return (((uint64_t)edx << 32) | eax);
}

void* fct(void*) { return NULL; }

int main(void) {
  uint64_t before;
  uint64_t after;
  pthread_t t;

  for (unsigned int i = 0; i < 10; ++i) {
    before = rdtscp();
    if (not fork()) { return 0; }
    after = rdtscp();
    std::cout << after - before  << std::endl;
  }

  return 0;
}
