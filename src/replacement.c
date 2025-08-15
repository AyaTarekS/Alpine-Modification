#include "replacement.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

void replacement_init(replace_t *replace, ReplacementAlgorithm algorithm, int capacity) {
    replace->algorithm = algorithm;
    replace->capacity = capacity;
    replace->frame_queue = (uint64_t *)malloc(capacity * sizeof(uint64_t));
    replace->last_access = (uint64_t *)malloc(capacity * sizeof(uint64_t));
    replace->count = 0;
    replace->head = 0;
    replace->tail = 0;
    replace->timeCounter = 0;

    for (int i = 0; i < capacity; i++) {
        replace->frame_queue[i] = UINT64_MAX;
        replace->last_access[i] = 0;
    }
}

void replacement_free(replace_t *replace) {
    free(replace->frame_queue);
    free(replace->last_access);
}

int find_page_index(replace_t *replace, uint64_t page_number) {
    // Search for the page in the frame queue
    for (int i = 0; i < replace->count; i++) {
        int idx = (replace->head + i) % replace->capacity;
        if (replace->frame_queue[idx] == page_number) {
            return idx;
        }
    }
    return -1;
}

int fifo_victim_index(replace_t *replace) {
    // dequeue
    int victim = replace->head;
    replace->head = (replace->head + 1) % replace->capacity;
    return victim;
}

int lru_victim_index(replace_t *replace) {
    uint64_t min_time = UINT64_MAX;
    int victim = -1;
    for (int i = 0; i < replace->count; i++) {
        int idx = (replace->head + i) % replace->capacity;
        if (replace->last_access[idx] < min_time) {
            min_time = replace->last_access[idx];
            victim = idx;
        }
    }
    return victim;
}

void page_access(replace_t *replace, uint64_t page_number) {
    int idx = find_page_index(replace, page_number);

    if (idx != -1) {
        // HIT: update access time for LRU
        replace->last_access[idx] = replace->timeCounter++;
        return;
    }

    // MISS (page fault)
    if (replace->count < replace->capacity) {
        // Insert in empty frame
        replace->frame_queue[replace->tail] = page_number;
        replace->last_access[replace->tail] = replace->timeCounter++;
        replace->tail = (replace->tail + 1) % replace->capacity;
        replace->count++;
    } else {
        // the memory is full we need to evict a page
        int victim_idx = (replace->algorithm == FIFO)
            ? fifo_victim_index(replace)
            : lru_victim_index(replace);

        replace->frame_queue[victim_idx] = page_number;
        replace->last_access[victim_idx] = replace->timeCounter++;
        printf("victim index: %llu\n" , victim_idx);

    }
    printf("Page access: %llu\n", page_number);
}
#include "replacement.h"
#include <stdio.h>

int main() {
    replace_t r_fifo, r_lru;

    replacement_init(&r_fifo, FIFO, 3);
    replacement_init(&r_lru, LRU, 3);

    printf("=== FIFO Test ===\n");
    page_access(&r_fifo, 1);
    page_access(&r_fifo, 2);
    page_access(&r_fifo, 3);
    page_access(&r_fifo, 4); 

    printf("\n=== LRU Test ===\n");
    page_access(&r_lru, 1);
    page_access(&r_lru, 2);
    page_access(&r_lru, 3);
    page_access(&r_lru, 1); 
    page_access(&r_lru, 4); 

    replacement_free(&r_fifo);
    replacement_free(&r_lru);
    return 0;
}

