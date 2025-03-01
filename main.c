#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "cut_list.h"
#include "piece_values.h"
#include "vec.h"

Cache *cache               = NULL;
static Vec global_val_list = NULL;

// Wrapper function for val_list
ValueType provider_wrapper(KeyType rod_length) {
    return optimal_cutlist_for(global_val_list, rod_length);
}

int main(int argc, char *argv[]) {
    // Check for correct number of arguments ex: ./rod_cutlru input.txt
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read piece values from file and store in a vector called val_list: value
    // list of pieces
    Vec val_list = read_piece_values(file);
    fclose(file);
    if (!val_list)
        return 1;

    // Set global val_list for the wrapper
    global_val_list = val_list;

    cache = load_cache_module("librr.so");  // librr.so for random replacement,
                                            // liblru.so for least recently used

    if (!cache) {
        fprintf(stderr, "Failed to load cache module. Exiting.\n");
        vec_free(val_list);
        return 1;
    }
    // Set provider
    ProviderFunction cached_cut = cache->set_provider_func(provider_wrapper);

    // function gets optimal cutlist for a given rod length, prints it, and
    // frees cache
    PieceLength rod_length;
    while (scanf("%d", &rod_length) == 1) {  // read rod lengths from stdin
        if (rod_length < 1) {
            fprintf(stderr, "Invalid rod length: %d\n", rod_length);
            continue;
        }

        CutList *optimal_cuts = cached_cut(rod_length);
        if (!optimal_cuts) {
            fprintf(stderr, "Error computing cuts for %d\n", rod_length);
            continue;
        }

        cutlist_print(optimal_cuts);
    }

    vec_free(val_list);
    cache->cache_cleanup();
    free(cache);
    return 0;
}
