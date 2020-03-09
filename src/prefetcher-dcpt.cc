/*
 * A DCPT prefetcher.
 */

#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "interface.hh"
#include "circular_buffer.hh"

using namespace std;

typedef int32_t Delta;
typedef std::vector<std::pair<std::pair<Delta, Delta>, size_t> > DeltaPairs;

const size_t ASSOCIATIVITY = 1;
const size_t BUFFER_LENGTH = 10;

struct Entry {
    Addr pc;
    Addr prev_addr;
    uint8_t buffer_index;
    uint32_t count;
    CircularBuffer<Delta> buffer;
};

const size_t ENTRY_SIZE = 2*sizeof(Addr) + sizeof(uint8_t) + BUFFER_LENGTH*sizeof(Delta) + sizeof(uint32_t);
const size_t TABLE_LENGTH = 1000;
Entry **table;

size_t addr_hash(Addr addr)
{
    return addr % TABLE_LENGTH;
}

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

    table = (Entry**) calloc(TABLE_LENGTH, sizeof(Entry*));

    for (size_t i = 0; i < TABLE_LENGTH; i++) {
	table[i] = (Entry*) calloc(ASSOCIATIVITY, sizeof(Entry));

	for (size_t j = 0; j < ASSOCIATIVITY; j++) {
	    table[i][j].pc = 0;
	    table[i][j].prev_addr = 0;
	    table[i][j].buffer_index = 0;
	    table[i][j].count = 0;
	    table[i][j].buffer.reset(BUFFER_LENGTH);
	}
    }

    DPRINTF(HWPrefetch, "Initialized DCPT prefetcher with table length %u and size %u bytes\n", TABLE_LENGTH, TABLE_LENGTH * ENTRY_SIZE * ASSOCIATIVITY);
}

void prefetch_access(AccessStat stat)
{
    size_t hash = addr_hash(stat.pc);
    Entry *entry = &table[hash][0];

    for (size_t i = 0; i < ASSOCIATIVITY; i++) {
	Entry *current = &table[hash][i];

	if (entry->pc == stat.pc || entry->pc == 0) {
	    entry = current;
	    break;
	} else if (current->count < entry->count) {
	    entry = current;
	}
    }

    //DPRINTF(HWPrefetch, "Access by PC %d on address %lu, with hash %lu\n", stat.pc, stat.mem_addr, hash);

    if (entry->pc == stat.pc) {
	// There's already an entry for this PC
	entry->count++;
	Delta delta = stat.mem_addr - entry->prev_addr;
	entry->prev_addr = stat.mem_addr;
	entry->buffer.push(delta);
	//DPRINTF(HWPrefetch, "Delta is %d\n", delta);
	//DPRINTF(HWPrefetch, "Buffer size is %d\n", entry->buffer.size());

	// Look for pairs
	if (entry->buffer.size() > 4) {
	    //DPRINTF(HWPrefetch, "Looking for pairs\n");
	    Delta left = entry->buffer[1];
	    Delta right = delta;

	    DeltaPairs pairs = entry->buffer.getPairs();

	    for (int i = 0; i < pairs.size() - 2; i++) {
		Delta buffer_left = pairs[i].first.first;
		Delta buffer_right = pairs[i].first.second;
		size_t index = pairs[i].second;

		if (left == buffer_left && right == buffer_right) {
		    //DPRINTF(HWPrefetch, "Found a match\n");
		    // We have a match. Prefetch using the deltas after this
		    Addr pf_addr = stat.mem_addr;
		    std::vector<Delta> range = entry->buffer.getRange(index+2);

		    for (int j = 0; j < range.size(); j++) {
			pf_addr += range[j];

			if (pf_addr > MAX_PHYS_MEM_ADDR) {
			    break;
			} else if (!in_cache(pf_addr) && !in_mshr_queue(pf_addr)) {
			    //DPRINTF(HWPrefetch, "Prefetching address %d\n", pf_addr);
			    issue_prefetch(pf_addr);
			}
		    }

		    break;
		}
	    }
	}
    } else {
	if (entry->pc == 0) {
	    //DPRINTF(HWPrefetch, "Creating a new entry\n");
	} else {
	    //DPRINTF(HWPrefetch, "Replacing entry for PC %u\n", entry->pc);
	}
	// There isn't an entry for this PC
	entry->pc = stat.pc;
	entry->prev_addr = stat.mem_addr;
	entry->count = 0;
	entry->buffer.reset();
    }
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
