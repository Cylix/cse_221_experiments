#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096
#define NB_BLOCKS 10000

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
  if (ac != 3) {
    std::cout << "./bin host port" << std::endl;
    return -1;
  }

  //! create socket
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    std::cout << "fail socket()" << std::endl;
    return -1;
  }

  struct sockaddr_in server;
  server.sin_addr.s_addr = inet_addr(av[1]);
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(av[2]));
  
  //! connect to server
  if (connect(fd, (const struct sockaddr *)&server, sizeof(server)) < 0) {
    std::cout << "fail connect()" << std::endl;
    return -1;
  }

  char buf[BUF_SIZE];
  unsigned int total = 0;
  unsigned int iter = 0;
  uint64_t start = rdtscp();
  while (total < BUF_SIZE * NB_BLOCKS) {
    total += recv(fd, buf, sizeof(buf), 0);
    ++iter;
  }
  uint64_t stop = rdtscp();

  std::cout << (stop - start - 5 * iter) << std::endl;
  
  close(fd);

  return 0;
}
