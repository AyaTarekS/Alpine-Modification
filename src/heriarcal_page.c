#include "heriarcal_page.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

pageTableLevel *create_page_table_level(int size) {
    pageTableLevel *level = malloc(sizeof(pageTableLevel));
    if (!level) return NULL;

    // Initialize all members
    level->next = calloc(size, sizeof(pageTableLevel *));
    if (!level->next) {
        free(level);
        return NULL;
    }

    level->frames = calloc(size, sizeof(int)); // Using calloc to initialize to 0
    if (!level->frames) {
        free(level->next);
        free(level);
        return NULL;
    }

    // Initialize all frames to -1 (invalid)
    for (int i = 0; i < size; i++) {
        level->frames[i] = -1;
    }

    return level;
}

void free_page_table_level(pageTableLevel *level, int size) {
    if (!level) return;

    // Recursively free child tables
    for (int i = 0; i < size; i++) {
        if (level->next[i]) {
            free_page_table_level(level->next[i], size);
        }
    }

    free(level->next);
    free(level->frames);
    free(level);
}

void address_translation(uint64_t virtual_address, pageTableLevel *page_table, Config *cfg) {
    // Input validation
    if (!page_table || !cfg) {
        fprintf(stderr, "Error: NULL pointer passed\n");
        return;
    }

    const int pageOffsetBits = (int)log2(cfg->pageSize);
    const int totalBits = cfg->addressBits;
    
    // Calculate dynamic bits per level
    int remainingBits = totalBits - pageOffsetBits;
    int bitsPerLevel = remainingBits / cfg->numberOfLevels;
    int extraBits = remainingBits % cfg->numberOfLevels;

    pageTableLevel *current = page_table;
    uint64_t addr = virtual_address >> pageOffsetBits;  // Remove offset

    for (int level = 0; level < cfg->numberOfLevels; level++) {
        int levelBits = bitsPerLevel + (level < extraBits ? 1 : 0);
        uint64_t indexMask = (1ULL << levelBits) - 1;
        int index = (addr >> (remainingBits - levelBits)) & indexMask;

        // Safety check
        if (index >= cfg->pageTableSize) {
            fprintf(stderr, "Error: VA 0x%lx requires table size ≥%d (current %d)\n",
                   virtual_address, index+1, cfg->pageTableSize);
            return;
        }

        if (!current->next[index]) {
            current->next[index] = create_page_table_level(cfg->pageTableSize);
        }
        current = current->next[index];
        remainingBits -= levelBits;
    }

    int frameIndex = addr % cfg->pageTableSize;
    current->frames[frameIndex] = addr;  // Store page number as frame number
    
    printf("Virtual Address: 0x%lx → Page: %lu → Frame: %d\n",
           virtual_address, addr, current->frames[frameIndex]);
}
