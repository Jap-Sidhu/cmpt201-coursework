// lab3.c
// Stores the last 5 lines of user input and prints them when the user types
// "print". Build: clang -std=c11 -Wall -Wextra -O0 -g lab3.c -o lab3

#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_CAP 5

typedef struct {
  char *lines[HISTORY_CAP];
  size_t count; // number of valid entries (0..HISTORY_CAP)
  size_t next;  // index where the next inserted item will go (oldest element)
} History;

static volatile sig_atomic_t g_interrupted = 0;

static void on_sigint(int sig) {
  (void)sig;
  g_interrupted = 1; // main loop will notice and exit cleanly
}

static void history_init(History *h) {
  for (size_t i = 0; i < HISTORY_CAP; ++i)
    h->lines[i] = NULL;
  h->count = 0;
  h->next = 0;
}

static void history_free(History *h) {
  for (size_t i = 0; i < HISTORY_CAP; ++i) {
    free(h->lines[i]);
    h->lines[i] = NULL;
  }
  h->count = 0;
  h->next = 0;
}

// Store a copy of `line` in the ring buffer, freeing any overwritten entry.
static void history_add(History *h, const char *line) {
  // Free the slot we're about to overwrite (if occupied)
  if (h->lines[h->next] != NULL) {
    free(h->lines[h->next]);
    h->lines[h->next] = NULL;
  }

  // strdup to persist independently of the getline buffer
  h->lines[h->next] = strdup(line);
  if (!h->lines[h->next]) {
    perror("strdup");
    exit(1);
  }

  h->next = (h->next + 1) % HISTORY_CAP;
  if (h->count < HISTORY_CAP) {
    h->count++;
  }
}

// Print from oldest -> newest.
// Oldest index = (count == HISTORY_CAP) ? next : 0.
static void history_print(const History *h) {
  if (h->count == 0)
    return;

  size_t start = (h->count == HISTORY_CAP) ? h->next : 0;
  for (size_t i = 0; i < h->count; ++i) {
    size_t idx = (start + i) % HISTORY_CAP;
    // Each stored line has no trailing newline; print with one.
    printf("%s\n", h->lines[idx] ? h->lines[idx] : "");
  }
}

int main(void) {
  // Handle Ctrl+C similar to the example (graceful exit).
  struct sigaction sa = {0};
  sa.sa_handler = on_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);

  History hist;
  history_init(&hist);

  char *buf = NULL; // getline-managed buffer
  size_t cap = 0;   // capacity (managed by getline)
  ssize_t nread;

  while (1) {
    // Prompt
    fputs("Enter input: ", stdout);
    fflush(stdout);

    nread = getline(&buf, &cap, stdin);
    if (nread < 0) {
      // EOF (Ctrl+D) or interrupted getline after SIGINT
      if (g_interrupted) {
        printf("\n(Ended with Ctrl+c)\n");
      }
      break;
    }

    // Strip one trailing '\n' if present (preserve blank lines)
    if (nread > 0 && buf[nread - 1] == '\n') {
      buf[nread - 1] = '\0';
      nread--;
    }

    // If the user pressed Ctrl+C during input, exit cleanly on next loop.
    if (g_interrupted) {
      printf("\n(Ended with Ctrl+c)\n");
      break;
    }

    if (strcmp(buf, "print") == 0) {
      // "print" is itself a user input and must be part of the last 5.
      history_add(&hist, buf);
      history_print(&hist);
    } else {
      // Any other input (including blanks, spaces, etc.)
      history_add(&hist, buf);
    }
  }

  free(buf);
  history_free(&hist);
  return 0;
}
