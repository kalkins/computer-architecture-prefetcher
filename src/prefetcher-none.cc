/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"


void prefetch_init(void)
{
}

void prefetch_access(AccessStat stat)
{
}

void prefetch_complete(Addr addr)
{
}
