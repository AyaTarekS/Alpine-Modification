#pragma once
#include <stddef.h>
#include <stdint.h> // This header defines uint64_t
#include "config.h"

typedef struct {
    ReplacementAlgorithm algorithm;
    int capacity;
    uint64_t *frame_queue; 
    uint64_t *last_access; //for the LRU 
    int count;
    int head;
    int tail;
    int timeCounter; //for LRU
    } replace_t;

void replacement_init(replace_t *replace, ReplacementAlgorithm algorithm, int capacity);
void find_page_access(replace_t *replace, uint64_t page_number);
void replacement_select_victim(replace_t *replace);
void replacement_free(replace_t *replace);
int fifo_victim_index(replace_t *replace);
int lru_victim_index(replace_t *replace);
void page_access(replace_t *replace, uint64_t page_number);