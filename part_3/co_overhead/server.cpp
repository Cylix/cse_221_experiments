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
main(int ac , char** av) {
  if (ac != 2) {
    std::cout << "./bin port" << std::endl;
    return -1;
  }

  // create socket
  int fd = socket(AF_INET , SOCK_STREAM , 0);
  if (fd < 0) {
    std::cout << "fail socket()" << std::endl;
    return -1;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(atoi(av[1]));

  // bind
  if(bind(fd, (const struct sockaddr *)&server , sizeof(server)) < 0) {
    std::cout << "fail bind()" << std::endl;
    return -1;
  }

  // listen
  listen(fd , 1024);

  //! get client
  int size = sizeof(struct sockaddr_in);
  struct sockaddr_in client;

  int client_fd;
  while ((client_fd = accept(fd, (struct sockaddr *)&client, (socklen_t*)&size))) {
    if (client_fd < 0) {
      std::cout << "fail accept()" << std::endl;
      return 1;
    }
    sleep(1);
    uint64_t start = rdtscp();
    close(client_fd);
    uint64_t end = rdtscp();
    std::cout << (end - start - 34) << std::endl;
  }
  
  close(fd);

  return 0;
}
