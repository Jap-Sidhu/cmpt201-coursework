#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN 256

int main() {
  char path[MAX_LEN];

  printf("Enter program to run.\n");

  while (1) {
    printf("> ");
    fflush(stdout);

    if (fgets(path, MAX_LEN, stdin) == NULL) {
      break;
    }

    path[strcspn(path, "\n")] = '\0';

    if (strlen(path) == 0) {
      continue;
    }

    pid_t pid = fork();

    if (pid < 0) {
      perror("fork failed");
      continue;
    }

    if (pid == 0) {
      execl(path, path, (char *)NULL);

      perror("Exec failure");
      exit(1);
    } else {
      int status;
      waitpid(pid, &status, 0);
    }
  }

  return 0;
}
