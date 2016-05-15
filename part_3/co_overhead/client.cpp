#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

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
  uint64_t start;
  uint64_t end;
  
  if (ac != 3) {
    std::cout << "./bin host port" << std::endl;
    return -1;
  }

  //! create socket
  start = rdtscp();
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  end = rdtscp();
  std::cout << "socket: " << (end - start - 34) << std::endl;
  if (fd < 0) {
    std::cout << "fail socket()" << std::endl;
    return -1;
  }

  struct sockaddr_in server;
  server.sin_addr.s_addr = inet_addr(av[1]);
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(av[2]));
  
  //! connect to server
  start = rdtscp();
  if (connect(fd, (const struct sockaddr *)&server, sizeof(server)) < 0) {
    std::cout << "fail connect()" << std::endl;
    return -1;
  }
  end = rdtscp();
  std::cout << "connect: " << (end - start - 34) << std::endl;

  start = rdtscp();
  close(fd);
  end = rdtscp();
  std::cout << "close: " << (end - start - 34) << std::endl;
  //while (1);
  
  return 0;
}
