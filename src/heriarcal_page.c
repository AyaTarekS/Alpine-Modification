#include "heriarcal_page.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
pageTableLevel *create_page_table_level(int size) {
    pageTableLevel *level = malloc(sizeof(pageTableLevel));
    if (!level) return NULL;

    // Allocate child pointers (initialize to NULL)
    level->next = calloc(size, sizeof(pageTableLevel *));
    if (!level->next) {
        free(level);
        return NULL;
    }

    // Allocate frame entries (initialize to -1 to mean "no frame assigned")
    level->frames = malloc(size * sizeof(int));
    if (!level->frames) {
        free(level->next);
        free(level);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        level->frames[i] = -1; // -1 means empty
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
    // Bits for page offset
    int pageOffsetBits = (int)log2(cfg->pageSize);
    // Bits per page table level
    int bitsPerLevel = (cfg->addressBits - pageOffsetBits) / cfg->numberOfLevels;

    pageTableLevel *current = root;

    // Go through each level of the page table
    for (int level = 0; level < cfg->numberOfLevels; level++) {
        // Shift amount for current level
        int shift = cfg->addressBits - pageOffsetBits - (bitsPerLevel * (level + 1));
        // Mask to extract the index bits for this level
        int indexMask = (1 << bitsPerLevel) - 1;
        int index = (virtual_address >> shift) & indexMask;

        // Allocate next level if needed
        if (current->next[index] == NULL) {
            current->next[index] = malloc(sizeof(pageTableLevel));
            memset(current->next[index], 0, sizeof(pageTableLevel));
        }

        // Move to the next level
        current = current->next[index];
    }

    // Final step: simulate mapping to a frame number
    // Here, we use the extracted index at the last level as the "frame number"
    int pageNumber = (virtual_address >> pageOffsetBits) & ((1 << (cfg->addressBits - pageOffsetBits)) - 1);
    current->frames[pageNumber % cfg->pageTableSize] = 1; // here we actually save the frame number

    printf("Virtual Address: 0x%lx -> Frame: %d\n", virtual_address, pageNumber % cfg->pageTableSize);
}

