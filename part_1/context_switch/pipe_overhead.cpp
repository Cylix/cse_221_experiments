#include <iostream>
#include <unistd.h>
#include <sys/time.h>

uint64_t
rdtscp(void) {
  uint32_t eax, edx;

  __asm__ __volatile__("rdtscp" //! rdtscp instruction
		       : "+a" (eax), "=d" (edx) //! output
		       : //! input
		       : "%ecx"); //! registers

  return (((uint64_t)edx << 32) | eax);
}

int main(void) {
  uint64_t before;
  uint64_t after;
  int fds[2];

  pipe(fds);

  for (unsigned int i = 0; i < 10; ++i) {
    before = rdtscp();
    write(fds[1], (char*)&before, sizeof(before));
    read(fds[0], (char*)&before, sizeof(before));
    after = rdtscp();

    std::cout << after - before  << std::endl;
  }

  return 0;
}
