#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct header {
  uint64_t size;
  struct header *next;
  int id;
};

void initialize_block(struct header *block, uint64_t size, struct header *next,
                      int id) {
  block->size = size;
  block->next = next;
  block->id = id;
}

// -----------------------------
// First-Fit Algorithm
// -----------------------------
int find_first_fit(struct header *free_list_ptr, uint64_t size) {
  struct header *curr = free_list_ptr;
  while (curr != NULL) {
    if (curr->size >= size)
      return curr->id;
    curr = curr->next;
  }
  return -1; // Not found
}

// -----------------------------
// Best-Fit Algorithm
// -----------------------------
int find_best_fit(struct header *free_list_ptr, uint64_t size) {
  int best_fit_id = -1;
  uint64_t min_diff = UINT64_MAX;

  struct header *curr = free_list_ptr;
  while (curr != NULL) {
    if (curr->size >= size) {
      uint64_t diff = curr->size - size;
      if (diff < min_diff) {
        min_diff = diff;
        best_fit_id = curr->id;
      }
    }
    curr = curr->next;
  }
  return best_fit_id;
}

// -----------------------------
// Worst-Fit Algorithm
// -----------------------------
int find_worst_fit(struct header *free_list_ptr, uint64_t size) {
  int worst_fit_id = -1;
  uint64_t max_diff = 0;

  struct header *curr = free_list_ptr;
  while (curr != NULL) {
    if (curr->size >= size) {
      uint64_t diff = curr->size - size;
      if (diff > max_diff) {
        max_diff = diff;
        worst_fit_id = curr->id;
      }
    }
    curr = curr->next;
  }
  return worst_fit_id;
}

// -----------------------------
// Main Function
// -----------------------------
int main(void) {

  struct header *free_block1 = (struct header*) malloc(sizeof(struct header));
  struct header *free_block2 = (struct header*) malloc(sizeof(struct header));
  struct header *free_block3 = (struct header*) malloc(sizeof(struct header));
  struct header *free_block4 = (struct header*) malloc(sizeof(struct header));
  struct header *free_block5 = (struct header*) malloc(sizeof(struct header));

  initialize_block(free_block1, 6, free_block2, 1);
  initialize_block(free_block2, 12, free_block3, 2);
  initialize_block(free_block3, 24, free_block4, 3);
  initialize_block(free_block4, 8, free_block5, 4);
  initialize_block(free_block5, 4, NULL, 5);

  struct header *free_list_ptr = free_block1;

  int first_fit_id = find_first_fit(free_list_ptr, 7);
  int best_fit_id = find_best_fit(free_list_ptr, 7);
  int worst_fit_id = find_worst_fit(free_list_ptr, 7);

  printf("The ID for First-Fit algorithm is: %d\n", first_fit_id);
  printf("The ID for Best-Fit algorithm is: %d\n", best_fit_id);
  printf("The ID for Worst-Fit algorithm is: %d\n", worst_fit_id);

  free(free_block1);
  free(free_block2);
  free(free_block3);
  free(free_block4);
  free(free_block5);

  return 0;
}

/*
--------------------------------------------
Part 2: Coalescing Contiguous Free Blocks
--------------------------------------------

Algorithm (pseudo-code):

1. Input: head pointer to the free list (linked list of free blocks).
2. Let curr = head.
3. While curr != NULL and curr->next != NULL:
      a. If the current block and the next block are contiguous in memory:
            - Combine (coalesce) them:
                curr->size = curr->size + sizeof(struct header) + curr->next->size
                curr->next = curr->next->next
         else:
            - Move to next block: curr = curr->next
4. End while
5. Return head (updated list with merged free blocks)

Explanation:
When a block is freed, we check adjacent blocks in the memory layout.
If two free blocks are next to each other (i.e., curr + curr->size == next),
we merge them to form a single larger free block to reduce fragmentation.

Example:
Before: [Block A][Used][Block B][Block C]
After freeing B: we check if A and B are contiguous.
If yes → merge into one larger [Block A+B].
Repeat for B+C if also contiguous.

End result: contiguous free blocks merged together to form fewer, larger blocks.
*/

