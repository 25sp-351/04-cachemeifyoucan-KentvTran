#ifndef LEAST_RECENTLY_USED_H
#define LEAST_RECENTLY_USED_H

#include "cache.h"

void initialize(void);
ProviderFunction set_provider(ProviderFunction downstream);
CacheStat* statistics(void);
void reset_statistics(void);
void cleanup(void);

#endif
