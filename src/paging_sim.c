#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    // Init root page table
    pageTableLevel root = {0};

    // Init replacement algorithm
    replace_t replace;
    int buffer_size = (cfg.replaceAlgo == FIFO) ? cfg.FIFO_buffer_size : cfg.LRU_buffer_size;
    replacement_init(&replace, cfg.replaceAlgo, buffer_size);

    // Simulate translations
    uint64_t virtual_addresses[] = {
        0x123456789ABC, 0xABCDEF123456, 0x111111111111, 0x222222222222
    };
    int count = sizeof(virtual_addresses) / sizeof(virtual_addresses[0]);

    for (int i = 0; i < count; i++) {
        printf("Translating VA: 0x%lx\n", virtual_addresses[i]);
        address_translation(virtual_addresses[i], &root, &cfg);
        page_access(&replace, virtual_addresses[i] >> (int)log2(cfg.pageSize));
    }

    // Cleanup
    replacement_free(&replace);

    return 0;
}
