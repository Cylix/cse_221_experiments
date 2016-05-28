#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
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
main(int ac , char** av) {
  if (ac != 3) {
    std::cout << "./bin port filename" << std::endl;
    return -1;
  }

  int file_fd = open(av[2], O_RDONLY);
  if (file_fd == -1) {
    std::cout << "could not open " << av[2] << std::endl;
    return -1;
  }
  
  fcntl(file_fd, F_NOCACHE, 1);
  unsigned int file_size = lseek(file_fd, 0, SEEK_END);
  lseek(file_fd, 0, SEEK_SET);

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
  listen(fd , 1);

  //! get client
  int size = sizeof(struct sockaddr_in);
  struct sockaddr_in client;
  int client_fd = accept(fd, (struct sockaddr *)&client, (socklen_t*)&size);
    
  if (client_fd < 0) {
    std::cout << "fail accept()" << std::endl;
    return 1;
  }

  char buf[BUF_SIZE];
  for (unsigned int j = 0; j < NB_READS; ++j) {
    lseek(file_fd, rand() % (size - BUF_SIZE), SEEK_SET);
    read(file_fd, buf, BUF_SIZE);
    send(client_fd, buf, sizeof(buf), 0);
  }
  std::cout << "file sent" << std::endl;
  
  close(client_fd);
  close(fd);
  close(file_fd);
  
  return 0;
}
