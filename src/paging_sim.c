#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "heriarcal_page.h"
#include "replacement.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s config_file\n", argv[0]);
        return 1;
    }

    // Load configuration
    Config cfg;
    load_config(argv[1], &cfg);

    // Properly initialize root page table
    pageTableLevel *root = create_page_table_level(cfg.pageTableSize);
    if (!root) {
        fprintf(stderr, "Failed to initialize root page table\n");
        return 1;
    }

    // Init replacement algorithm
    replace_t replace;
    int buffer_size = (cfg.replaceAlgo == FIFO) ? cfg.FIFO_buffer_size : cfg.LRU_buffer_size;
    replacement_init(&replace, cfg.replaceAlgo, buffer_size);

    // Validate configuration
    if (cfg.pageSize <= 0 || cfg.numberOfLevels <= 0 || cfg.addressBits <= 0) {
        fprintf(stderr, "Invalid configuration values\n");
        free_page_table_level(root, cfg.pageTableSize);
        return 1;
    }

    // Simulate translations with more reasonable addresses
    uint64_t virtual_addresses[] = {
        0x1000,  // Smaller addresses for 32-bit systems
        0x2000,
        0x3000,
        0x4000
    };
    int count = sizeof(virtual_addresses) / sizeof(virtual_addresses[0]);

    for (int i = 0; i < count; i++) {
        printf("\nTranslating VA: 0x%lx\n", virtual_addresses[i]);
        address_translation(virtual_addresses[i], root, &cfg);
        uint64_t page_number = virtual_addresses[i] >> (int)log2(cfg.pageSize);
        page_access(&replace, page_number);
    }

    // Cleanup
    replacement_free(&replace);
    free_page_table_level(root, cfg.pageTableSize);

    return 0;
}
