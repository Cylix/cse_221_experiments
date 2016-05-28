#include <iostream>
#include <cmath>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 4096
#define NB_READS 10000

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
    fcntl(fd, F_NOCACHE, 1);
    unsigned int size = lseek(fd, 0, SEEK_END) + 1;
    lseek(fd, 0, SEEK_SET);

    char buf[BUF_SIZE];
    unsigned int total = 0;

    for (unsigned int j = 0; j < NB_READS; ++j) {
      lseek(fd, rand() % (size - BUF_SIZE), SEEK_SET);

      uint64_t before = rdtscp();
      read(fd, buf, BUF_SIZE);
      uint64_t after = rdtscp();
      total += (after - before) - 34;
    }

    std::cout << (total / NB_READS) << std::endl;
        
    close(fd);
    ++i;
  }
  
  return 0;
}

