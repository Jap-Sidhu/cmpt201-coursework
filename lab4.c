#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define BUF_SIZE 128

struct header {
    uint64_t size;
    struct header *next;
};

void handle_error(const char *msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
    _exit(1);
}

void print_out(const char *format, void *data, size_t data_size) {
    char buf[BUF_SIZE];
    ssize_t len = snprintf(
        buf, BUF_SIZE, format,
        data_size == sizeof(uint64_t) ? *(uint64_t *)data : *(void **)data
    );
    if (len < 0) handle_error("snprintf error\n");
    write(STDOUT_FILENO, buf, len);
}

int main() {
    size_t total_size = 256;
    void *heap_start = sbrk(total_size);
    if (heap_start == (void *)-1) handle_error("sbrk failed\n");

    struct header *first = (struct header *)heap_start;
    struct header *second = (struct header *)((char *)heap_start + 128);

    // Initialize headers
    first->size = 128;
    first->next = NULL;

    second->size = 128;
    second->next = first;

    // Data regions
    size_t data_size = 128 - sizeof(struct header);
    char *first_data = (char *)(first + 1);
    char *second_data = (char *)(second + 1);

    memset(first_data, 0, data_size);
    memset(second_data, 1, data_size);

    // Print addresses
    print_out("first block: %p\n", &first, sizeof(first));
    print_out("second block: %p\n", &second, sizeof(second));

    // Print header values
    print_out("first block size: %" PRIu64 "\n", &first->size, sizeof(first->size));
    print_out("first block next: %p\n", &first->next, sizeof(first->next));
    print_out("second block size: %" PRIu64 "\n", &second->size, sizeof(second->size));
    print_out("second block next: %p\n", &second->next, sizeof(second->next));

    // Print data
    for (size_t i = 0; i < data_size; i++) {
        char buf[16];
        int len = snprintf(buf, sizeof(buf), "%d\n", first_data[i]);
        write(STDOUT_FILENO, buf, len);
    }
    for (size_t i = 0; i < data_size; i++) {
        char buf[16];
        int len = snprintf(buf, sizeof(buf), "%d\n", second_data[i]);
        write(STDOUT_FILENO, buf, len);
    }

    return 0;
}

