/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"
#include <stdio.h>

struct rpt_entry {
	int prev_addr;
	int stride;
};
rpt_entry prefetch_table [128] = {};

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

    DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
    printf("Handling a request with %d PC and %d hit state\n", stat.pc, stat.miss);
    int pc_index = (stat.pc >> 2) % 128;
    rpt_entry cur_entry = prefetch_table[pc_index];

    cur_entry.stride = stat.mem_addr - cur_entry.prev_addr;
    cur_entry.prev_addr = stat.mem_addr;
    prefetch_table[pc_index] = cur_entry;

    int fetch_dest = cur_entry.prev_addr + cur_entry.stride;
    if(!in_cache(fetch_dest) && !in_mshr_queue(fetch_dest)) {
	    if(fetch_dest < 0 || fetch_dest > MAX_PHYS_MEM_ADDR) {
		    fetch_dest = stat.mem_addr + BLOCK_SIZE;
	    }
	    issue_prefetch(fetch_dest);
    }
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}