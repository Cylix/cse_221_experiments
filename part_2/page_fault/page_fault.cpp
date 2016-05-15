#include <iostream>
#include <sys/mman.h>
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

int
main(void) {
  uint64_t before;
  uint64_t after;

  char* first = (char *)mmap(NULL, getpagesize(), PROT_READ, MAP_PRIVATE | MAP_ANON, 0, 0);
  for (unsigned int i = 0; i < 1000000; ++i)
    mmap(NULL, getpagesize(), PROT_READ, MAP_PRIVATE | MAP_ANON, 0, 0);

  before = rdtscp();
  char c = *first;
  after = rdtscp();
  std::cout << after - before << std::endl;
  
  return 0;
}
