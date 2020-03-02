/*
 * A DCPT prefetcher.
 */

#include <stdlib.h>
#include <stdint.h>
#include "interface.hh"

using namespace std;

typedef int16_t Delta;
typedef uint8_t Index;

const size_t BUFFER_LENGTH = 8;
struct Entry {
    Addr pc;
    Addr prev_addr;
    uint8_t buffer_index;
    Delta buffer[BUFFER_LENGTH];
};

const size_t TABLE_LENGTH = sizeof(Entry) / BUFFER_LENGTH;
Entry table[TABLE_LENGTH];

int in_flight(Addr addr)
{
    // TODO: Implement this
    return 0;
}

size_t addr_hash(Addr addr)
{
    return addr / TABLE_LENGTH;
}

Index add_index(Index index, int n)
{
    return index + n % BUFFER_LENGTH;
}

Index inc_index(Index index)
{
    return add_index(index, 1);
}

Index dec_index(Index index)
{
    return add_index(index, -1);
}

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

    //DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
    size_t hash = addr_hash(stat.mem_addr);
    Entry &entry = table[hash];

    if (entry.pc == stat.pc) {
	// There's already an entry for this PC
	Delta delta = stat.mem_addr - entry.prev_addr;
	entry.prev_addr = stat.mem_addr;
	entry.buffer[entry.buffer_index] = delta;

	// Look for pairs
	Delta left = entry.buffer[dec_index(entry.buffer_index)];
	Delta right = delta;
	Index last = add_index(entry.buffer_index, -2);
	for (Index i = inc_index(entry.buffer_index); i != last; i = inc_index(i)) {
	    if (left == entry.buffer[i] && right == entry.buffer[inc_index(i)]) {
		// We have a match. Prefetch using the deltas after this
		Addr pf_addr = stat.mem_addr;
		for (Index j = add_index(i, 2); j != entry.buffer_index; j = inc_index(j)) {
		    pf_addr += entry.buffer[j];

		    if (!in_cache(pf_addr) && !in_flight(pf_addr)) {
			issue_prefetch(pf_addr);
		    }
		}
		break;
	    }
	}

	entry.buffer_index = inc_index(entry.buffer_index);
    } else {
	// There isn't an entry for this PC
	entry.pc = stat.pc;
	entry.prev_addr = stat.mem_addr;
	entry.buffer_index = 0;

	for (Index i = 0; i < BUFFER_LENGTH; i++) {
	    entry.buffer[i] = 0;
	}
    }
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
