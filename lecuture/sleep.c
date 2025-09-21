#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  printf("Hello World!\n");
  pid_t pid = fork();

  if (pid > 0) {
    execl("/usr/bin/ls", "ls", NULL);
  } else if (pid == 0) {
    execl("/usr/bin/", "ls-alh", NULL);
  }
  return 0;
}
