#include <iostream>
#include <cmath>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 4096
#define NB_LOOPS 10

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
  unsigned int i = 1;

  int fd;
  while ((fd = open(("data_" + std::to_string(i)).c_str(), O_RDONLY)) != -1) {
    char buf[BUF_SIZE];
    unsigned int total = 0;

    for (unsigned int j = 0; j < NB_LOOPS; ++j) {
      unsigned int nb_reads = 0;
      uint64_t before = rdtscp();
      while (read(fd, buf, BUF_SIZE) == BUF_SIZE)
	++nb_reads;
      uint64_t after = rdtscp();
      total += (after - before - (nb_reads * 5.2) - 48 - 34) / nb_reads;
      lseek(fd, 0, SEEK_SET);
    }

    std::cout << i << "G: " << (total / NB_LOOPS) << std::endl;
        
    close(fd);
    ++i;
  }
  
  return 0;
}

