#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

uint64_t
rdtscp(void) {
  uint32_t eax, edx;

  __asm__ __volatile__("rdtscp" //! rdtscp instruction
		       : "+a" (eax), "=d" (edx) //! output
		       : //! input
		       : "%ecx"); //! registers

  return (((uint64_t)edx << 32) | eax);
}

int fds[2];

void* fct(void* id_) {
  int* id = (int*) id_;
  uint64_t before;
  uint64_t after;
  char c;

  if (*id == 1) {
      read(fds[0], (char*)&before, sizeof(before));
      after = rdtscp();
      std::cout << after - before << std::endl;
    }
  else {
      before = rdtscp();
      write(fds[1], (char*)&before, sizeof(before));
    }
}

int main(void) {
  int thread_1 = 1;
  int thread_2 = 2;
  pthread_t t;

  pipe(fds);

  pthread_create(&t, NULL, &fct, &thread_2);
  fct(&thread_1);

  return 0;
}
