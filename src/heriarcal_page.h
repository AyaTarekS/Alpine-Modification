#pragma once
#include "config.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct pageTableLevel {
    struct pageTableLevel **next;
    int *frames[cfg->pageTableSize];
} pageTableLevel;


pageTableLevel *create_page_table_level(int size);

void free_page_table_level(pageTableLevel *level, int size);


void address_translation(uint64_t virtual_address, pageTableLevel *page_table, Config *cfg);
