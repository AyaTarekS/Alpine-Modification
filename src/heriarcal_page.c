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

void address_translation(uint64_t virtual_address, pageTableLevel *root, Config *cfg) {
    // Validate inputs
    if (!root || !cfg) {
        fprintf(stderr, "Error: NULL pointer passed to address_translation\n");
        return;
    }

    if (cfg->pageSize <= 0 || cfg->numberOfLevels <= 0 || cfg->addressBits <= 0) {
        fprintf(stderr, "Error: Invalid configuration values\n");
        return;
    }

    // Calculate bits for page offset
    int pageOffsetBits = (int)log2(cfg->pageSize);
    if (pageOffsetBits <= 0 || pageOffsetBits >= cfg->addressBits) {
        fprintf(stderr, "Error: Invalid pageOffsetBits calculation\n");
        return;
    }

    // Calculate bits per level
    int remainingBits = cfg->addressBits - pageOffsetBits;
    int bitsPerLevel = remainingBits / cfg->numberOfLevels;
    
    if (bitsPerLevel <= 0) {
        fprintf(stderr, "Error: Invalid bitsPerLevel calculation\n");
        return;
    }

    pageTableLevel *current = root;

    // Traverse each level of the page table
    for (int level = 0; level < cfg->numberOfLevels; level++) {
        int shift = remainingBits - (bitsPerLevel * (level + 1));
        int indexMask = (1 << bitsPerLevel) - 1;
        int index = (virtual_address >> shift) & indexMask;

        // Verify index is within bounds
        if (index < 0 || index >= cfg->pageTableSize) {
            fprintf(stderr, "Error: Invalid index calculation at level %d\n", level);
            return;
        }

        // Allocate next level if needed
        if (!current->next[index]) {
            current->next[index] = create_page_table_level(cfg->pageTableSize);
            if (!current->next[index]) {
                fprintf(stderr, "Error: Failed to allocate page table level\n");
                return;
            }
        }

        current = current->next[index];
    }

    // Calculate frame number
    int pageNumber = (virtual_address >> pageOffsetBits);
    int frameIndex = pageNumber % cfg->pageTableSize;
    
    // Validate frame index
    if (frameIndex < 0 || frameIndex >= cfg->pageTableSize) {
        fprintf(stderr, "Error: Invalid frame index calculation\n");
        return;
    }

    // Assign frame (using pageNumber as the frame number for simulation)
    current->frames[frameIndex] = pageNumber;
    printf("Virtual Address: 0x%lx -> Page: %d -> Frame: %d\n", 
           virtual_address, pageNumber, current->frames[frameIndex]);
}
