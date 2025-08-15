#pragma once
typedef enum {FIFO = 0, LRU = 1} ReplacementAlgorithm;
typedef struct{
    int pageSize;
    int numberOfLevels;
    ReplacementAlgorithm replaceAlgo;
    int FIFO_buffer_size;
    int LRU_buffer_size;
    int addressBits;
    int pageTableSize;
} Config;

//to load the configuration from a file
void load_config(const char* filename, Config* configuration);