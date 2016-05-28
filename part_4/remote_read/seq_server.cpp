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

#define BUF_SIZE (4096 * 32)

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

  unsigned int i = 1;

  int file_fd;
  while ((file_fd = open(("data_" + std::to_string(i)).c_str(), O_RDONLY)) != -1) {
    fcntl(file_fd, F_NOCACHE, 1);
    unsigned int file_size = lseek(file_fd, 0, SEEK_END) + 1;
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

    //! send size
    send(client_fd, (char*)&file_size, sizeof(file_size), 0);

    char buf[BUF_SIZE];
    while (read(file_fd, buf, BUF_SIZE) == BUF_SIZE)
      send(client_fd, buf, sizeof(buf), 0);

    close(client_fd);
    close(fd);
    close(file_fd);
  }
  
  return 0;
}
