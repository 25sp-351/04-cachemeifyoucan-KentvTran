#include "random_replacement.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bsd/stdlib.h> 

#include "cut_list.h"

#define CACHE_SIZE 10  // small for easy testing

typedef struct CacheEntry {
    KeyType key;
    ValueType value;
    bool valid;  // flag to check if the entry is valid
} CacheEntry;

static CacheEntry cache[CACHE_SIZE];
static int cache_requests         = 0;
static int cache_hits             = 0;
static int cache_evictions        = 0;
static ProviderFunction _provider = NULL;

// Function to retrieve a value from the cache
// input: key id of the value being requested
// output: returns the value from the cache if it is present, otherwise it
// retrieves the value from the provider function evicts if cache is over
// capacity, and keeps track of cache statistics debug print statements for all
// cache actions
ValueType _caching_provider(KeyType key) {
    cache_requests++;

    // Check if in cache
    for (int ix = 0; ix < CACHE_SIZE; ix++) {
        if (cache[ix].valid && cache[ix].key == key) {
            cache_hits++;

#ifdef DEBUG
            printf("[RANDOM] HIT key=%-4d \n", key);
#endif

            return cache[ix].value;
        }
    }

// Cache miss - use the provider function
#ifdef DEBUG
    printf("[RANDOM] MISS key=%-4d\n", key);
#endif
    ValueType result = _provider(key);

    // Store in random position
    int index = arc4random_uniform(CACHE_SIZE);

    // Evict existing entry if random position is occupied
    evict_cache_entry(index);

    cache[index].key   = key;
    cache[index].value = result;
    cache[index].valid = true;

#ifdef DEBUG
    printf("[RANDOM] ADD key=%-4d \n", key);
#endif

    return result;
}

// input: downstream provider function (function that will be cached)
// output: returns the caching provider function that will be used to call
// original function to retrieve values
ProviderFunction set_provider(ProviderFunction downstream) {
    srand(time(NULL));
    _provider = downstream;
    return _caching_provider;
}

void evict_cache_entry(int index) {
    if (cache[index].valid) {
        cutlist_free(cache[index].value);
        cache_evictions++;

#ifdef DEBUG
        printf("[RANDOM] EVICT key=%-4d \n", cache[index].key);
#endif

        cache[index].valid = false;  // Mark entry as invalid
    }
}

// stores states of the cache
CacheStat *statistics(void) {
    CacheStat *stats = malloc(5 * sizeof(CacheStat));
    stats[0]         = (CacheStat){Cache_requests, cache_requests};
    stats[1]         = (CacheStat){Cache_hits, cache_hits};
    stats[2]         = (CacheStat){Cache_misses, cache_requests - cache_hits};
    stats[3]         = (CacheStat){Cache_evictions, cache_evictions};
    stats[4]         = (CacheStat){END_OF_STATS, 0};
    return stats;
}

void reset_statistics(void) {
    cache_requests  = 0;
    cache_hits      = 0;
    cache_evictions = 0;
}

void cleanup() {
    for (int ix = 0; ix < CACHE_SIZE; ix++) {
        if (cache[ix].valid) {
            CutList *cl = (CutList *)cache[ix].value;
            cutlist_free(cl);
            cache[ix].valid = false;
        }
    }
}

void initialize() {
}
