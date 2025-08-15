#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"


void load_config(const char* filename, Config* configuration) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Cannot open config file %s\n", filename);
        return;
    }

    char line[100]; //parsing the file to save it into line buffer
    while (fgets(line, sizeof(line), fp)) {
        char key[50], value[50]; //separating it to have key-value pairs
        if (sscanf(line, "%49[^=]=%49s", key, value) == 2) {
            //strcmp returns 0 if strings are equal
            if (strcmp(key, "pageSize") == 0) {
                configuration->pageSize = atoi(value);
            }
            else if (strcmp(key, "numberOfLevels") == 0) {
                configuration->numberOfLevels = atoi(value);
            }
            else if (strcmp(key, "replaceAlgo") == 0) {
                configuration->replaceAlgo = (strcmp(value, "FIFO") == 0) ? FIFO : LRU;
            }
            else if (strcmp(key, "FIFO_buffer_size") == 0) {
                configuration->FIFO_buffer_size = atoi(value);
            }
            else if (strcmp(key, "LRU_buffer_size") == 0) {
                configuration->LRU_buffer_size = atoi(value);
            }
            else if (strcmp(key, "addressBits") == 0) {
                configuration->addressBits = atoi(value);
            }
            else if (strcmp(key, "pageTableSize") == 0) {
                configuration->pageTableSize = atoi(value);
            }
        }
    }

    fclose(fp);
}




