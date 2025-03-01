#include "least_recently_used.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cache.h"

#define CACHE_SIZE 10  // small for easy testing

typedef struct CacheNode {
    KeyType key;
    ValueType value;
    struct CacheNode *prev;
    struct CacheNode *next;
} CacheNode;

static CacheNode *head            = NULL;
static CacheNode *tail            = NULL;
static int current_size           = 0;

static ProviderFunction _provider = NULL;
static int cache_requests         = 0;
static int cache_hits             = 0;
static int cache_evictions        = 0;

CacheNode *create_node(KeyType key, ValueType value) {
    CacheNode *node = malloc(sizeof(CacheNode));
    node->key       = key;
    node->value     = value;
    node->prev      = NULL;
    node->next      = NULL;

#ifdef DEBUG
    printf("[LRU] ADD key=%-4d\n", tail->key);
#endif

    return node;
}

void move_to_head(CacheNode *node) {
    if (node == head)
        return;
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    if (node == tail)
        tail = node->prev;
    node->prev = NULL;
    node->next = head;
    if (head)
        head->prev = node;
    head = node;
    if (!tail)
        tail = head;
}

void evict_last() {
    if (!tail)
        return;

#ifdef DEBUG
    printf("[LRU] EVICT key=%-4d\n", tail->key);
#endif

    CacheNode *temp = tail;
    if (tail->value)
        cutlist_free(tail->value);
    if (tail->prev) {
        tail       = tail->prev;
        tail->next = NULL;
    } else {
        head = tail = NULL;
    }
    free(temp);
    current_size--;
    cache_evictions++;
}

void add_to_cache(KeyType key, ValueType value) {
    CacheNode *node = create_node(key, value);
    if (current_size >= CACHE_SIZE)
        evict_last();
    move_to_head(node);
    current_size++;
}

// Returns the value associated with the key, or NULL if not found
ValueType get(KeyType key) {
    CacheNode *current = head;
    while (current) {
        if (current->key == key) {
#ifdef DEBUG
            printf("[LRU] HIT key=%-4d \n", key);
#endif

            cache_hits++;
            move_to_head(current);
            return current->value;
        }
        current = current->next;
    }

#ifdef DEBUG
    printf("[LRU] MISS key=%-4d\n", key);
#endif

    return NULL;
}

// Function to retrieve a value from the cache
// input: key id of the value being requested
// output: returns the value from the cache if it is present, otherwise it
// retrieves the value from the provider function
ValueType _caching_provider(KeyType key) {
    cache_requests++;
    ValueType cached = get(key);
    if (cached)
        return cached;

    ValueType result = _provider(key);
    add_to_cache(key, result);
    return result;
}

// input: downstream provider function (function that will be cached)
// output: returns the caching provider function that will be used to call
// original function to retrieve values
ProviderFunction set_provider(ProviderFunction downstream) {
    _provider = downstream;
    return _caching_provider;
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
    CacheNode *current = head;
    while (current) {
        CacheNode *next = current->next;
        CutList *cl     = (CutList *)current->value;
        cutlist_free(cl);
        free(current);
        current = next;
    }
    head = tail  = NULL;
    current_size = 0;
}

void initialize() {
}
