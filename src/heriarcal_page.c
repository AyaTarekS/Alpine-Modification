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
    // Validate inputs
    if (!page_table || !cfg) {
        fprintf(stderr, "Error: NULL pointer passed\n");
        return;
    }

    // Calculate bits for page offset
    int pageOffsetBits = (int)log2(cfg->pageSize);
    int remainingBits = cfg->addressBits - pageOffsetBits;
    
    // Verify configuration
    if (pageOffsetBits <= 0 || remainingBits <= 0 || 
        cfg->numberOfLevels <= 0 || cfg->pageTableSize <= 0) {
        fprintf(stderr, "Error: Invalid configuration\n");
        return;
    }

    // Calculate bits per level (distribute evenly)
    int bitsPerLevel = remainingBits / cfg->numberOfLevels;
    int extraBits = remainingBits % cfg->numberOfLevels;
    
    pageTableLevel *current = page_table;
    uint64_t mask = (1ULL << remainingBits) - 1;
    uint64_t pageNumber = (virtual_address >> pageOffsetBits) & mask;

    for (int level = 0; level < cfg->numberOfLevels; level++) {
        // Distribute extra bits to earlier levels
        int levelBits = bitsPerLevel + (level < extraBits ? 1 : 0);
        int shift = remainingBits - levelBits;
        
        uint64_t indexMask = (1ULL << levelBits) - 1;
        int index = (pageNumber >> shift) & indexMask;

        // Validate index
        if (index >= cfg->pageTableSize) {
            fprintf(stderr, "Error: Index %d exceeds table size %d at level %d\n",
                   index, cfg->pageTableSize, level);
            return;
        }

        // Allocate next level if needed
        if (!current->next[index]) {
            current->next[index] = create_page_table_level(cfg->pageTableSize);
            if (!current->next[index]) {
                fprintf(stderr, "Error: Allocation failed at level %d\n", level);
                return;
            }
        }

        current = current->next[index];
        remainingBits -= levelBits;
    }

    // Store frame mapping
    int frameIndex = pageNumber % cfg->pageTableSize;
    current->frames[frameIndex] = pageNumber;
    
    printf("Virtual Address: 0x%lx -> Page: %lu -> Frame: %d\n",
           virtual_address, pageNumber, current->frames[frameIndex]);
}
