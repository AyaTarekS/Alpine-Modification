#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "heriarcal_page.h"
#include "replacement.h"

void run_comprehensive_tests(Config *cfg, pageTableLevel *root, replace_t *replace) {
    // Test cases covering all scenarios
    uint64_t test_cases[] = {
        // Basic functionality
        0x0000,     // Zero address
        0x1000,     // Page 1 
        0x1FFF,     // Last byte of page 1
        0x2000,     // Page 2
        0x3FFF,     // Last byte of page 3
        
        // Edge cases
        0x00000000, // Minimum 32-bit address
        0xFFFFF000, // Maximum page-aligned address
        0xDEADBEEF, // Memorable pattern
        0xCAFEBABE, // Another pattern
        
        // Replacement stress tests
        0x5000,     
        0x6000,
        0x7000,
        0x5000,     // Should hit if replacement works
        0x8000      // Should trigger eviction
    };

    printf("=== Starting Comprehensive Paging Tests ===\n");
    for(int i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        printf("\nTest %d: VA = 0x%lx\n", i+1, test_cases[i]);
        address_translation(test_cases[i], root, cfg);
        page_access(replace, test_cases[i] >> (int)log2(cfg->pageSize));
    }
    printf("\n=== All tests completed ===\n");
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s config_file\n", argv[0]);
        return 1;
    }



    // Initialize page table with proper error checking
    pageTableLevel *root = create_page_table_level(cfg.pageTableSize);
    if(!root) {
        fprintf(stderr, "Failed to initialize page table\n");
        return 1;
    }

    // Initialize replacement algorithm
    replace_t replace;
    int buffer_size = (cfg.replaceAlgo == FIFO) ? cfg.FIFO_buffer_size : cfg.LRU_buffer_size;

    // Run tests
    run_comprehensive_tests(&cfg, root, &replace);

    // Cleanup
    replacement_free(&replace);
    free_page_table_level(root, cfg.pageTableSize);
    
    return 0;
}

