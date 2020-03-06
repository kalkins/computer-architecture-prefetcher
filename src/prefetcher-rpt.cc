/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"

struct rpt_entry {
	//Addr PC;
	int prev_addr;
	int stride;
	//char state;					// 'i' = init, 't' = transient, 's' = steady, 'n' = no-prediction
};
rpt_entry rpt [128] = {};

int addr_hash(Addr addr)
{
    return addr % 128;
}

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */
	
	

    //DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
    
	Addr pc = stat.pc;
	rpt_entry entry = rpt[addr_hash(pc)];
	//bool pf_en = false;
	
	if(entry.prev_addr){
		if(entry.stride == stat.mem_addr - entry.prev_addr){
			Addr pf_addr = stat.mem_addr + entry.stride * BLOCK_SIZE;
			if(pf_addr >= 0 && pf_addr <= MAX_PHYS_MEM_ADDR) {
				if (!in_cache(pf_addr) && !in_mshr_queue(pf_addr)) {
					issue_prefetch(pf_addr);
				}
			}
			
		}
		else {
			entry.stride = stat.mem_addr - entry.prev_addr;
		}
	}
	else{
		entry.prev_addr = stat.mem_addr;
		entry.stride = 0; // should anything be stored here?
	}
	
	
	/* In case of 4-state system propose by Chen & Baer: */
	
	/*switch(curr_entry.state){			
		case 'i':
			if(entry.prev_addr){
				entry.stride = stat.mem_addr - entry.prev_addr;
				entry.prev_addr = stat.mem_addr;
				entry.state = 't';
			}
			else{
				entry.prev_addr = stat.mem_addr;
			}
			break;
		case 't':
			if(entry.stride == stat.mem_addr - entry.prev_addr){
				entry.state = 's';
				pf_en = true;
			}
			else{
				entry.stride = stat.mem_addr - entry.prev_addr;
				entry.state = 'n';
			}
			break;
		case 's':
			if(entry.stride == stat.mem_addr - entry.prev_addr){
				pf_en = true;
			}
			else{
				//entry.stride = stat.mem_addr - entry.prev_addr;
				// Remove entry info?
				entry.state = 'i';
			}
			break;
		case 'n':
			if(entry.stride == stat.mem_addr - entry.prev_addr){
				entry.state = 't';
				pf_en = true;
			}
			else{
				entry.stride = stat.mem_addr - entry.prev_addr;
			}
			break;
		default:
			entry.prev_addr = stat.mem_addr;
			entry.state = 'i';
	}*/
	
	/* pf_addr is now an address within the cache block that is a stride later
    Addr pf_addr = stat.mem_addr + stride * BLOCK_SIZE;

    if (stat.miss && !in_cache(pf_addr) && pf_en) {
        issue_prefetch(pf_addr);
    }*/
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
