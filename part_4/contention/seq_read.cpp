#include <iostream>
#include <cmath>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE (4096 * 32)
#define NB_LOOPS 3

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
main(int ac, char** av) {
  if (ac != 2) {
    std::cout << av[0] << " filename" << std::endl;
    return -1;
  }
  
  int fd = open(av[1], O_RDONLY);
  if (fd == -1) {
    std::cout << "could not open file " << av[1] << std::endl;
    return -1;
  }

  fcntl(fd, F_NOCACHE, 1);
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

  #ifdef DISP
  std::cout << (total / NB_LOOPS) << std::endl;
  #endif
        
  close(fd);
  
  return 0;
}

