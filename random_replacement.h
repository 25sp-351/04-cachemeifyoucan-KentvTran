#ifndef RANDOM_REPLACEMENT_H
#define RANDOM_REPLACEMENT_H

#include "cache.h"

void initialize(void);
ProviderFunction set_provider(ProviderFunction downstream);
void evict_cache_entry(int index);
CacheStat* statistics(void);
void reset_statistics(void);
void cleanup(void);

#endif
