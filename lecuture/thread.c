#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

void *thread_func(void *arg) {
  char *str = (char *)arg;
  pid_t tid = pthread_self();
  printf("New thread (pthread PID: %d)\n", tid);

  size_t len = strlen(str);
  return (void *)(uintptr_t)len;
}

int main() {
  pthread_t thread;
  void *ret_val;

  char message[] = "Hello Pathreads";
  printf("Main thread: creating new thread\n");

  if (pthread_create(&thread, NULL, thread_func, (void *)message) != 0) {
    perror("failed to create thread");
    exit(1);
  }

  pthread_join(thread, &ret_val);

  printf("Main thread: value returned\n");
  return 0;
}
