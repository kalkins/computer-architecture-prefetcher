/*
 * Reference Prediction Table
 */

#include "interface.hh"

struct rpt_entry {
	//Addr PC;
	Addr prev_addr;
	int64_t stride;
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

	for(int i = 0; i < 128; i++){
		rpt[i].prev_addr = 0;
		rpt[i].stride = 0;
	}


    //DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

void prefetch_access(AccessStat stat)
{

	Addr pc = stat.pc;
	rpt_entry &entry = rpt[addr_hash(pc)];
	DPRINTF(HWPrefetch, "PC %lu: Entry with previous address %lu, and stride %lu\n", stat.pc, entry.prev_addr, entry.stride);

	/* Prefetch addr + stride if the current stride matches the previous stride. */

	if(entry.prev_addr != 0){
		int64_t current_stride = stat.mem_addr - entry.prev_addr;
		DPRINTF(HWPrefetch, "Check the stride against current stride %lu\n", current_stride);

		if(current_stride == entry.stride){
			DPRINTF(HWPrefetch, "Stride correlates\n");
			Addr pf_addr = stat.mem_addr + entry.stride;

			if(pf_addr >= 0 && pf_addr <= MAX_PHYS_MEM_ADDR) {
				if (!in_cache(pf_addr) && !in_mshr_queue(pf_addr)) {
					issue_prefetch(pf_addr);
				}
			}
		}
		else {
			entry.stride = current_stride;
		}
	}

	entry.prev_addr = stat.mem_addr;


	/* In case of 4-state system proposed by Chen & Baer: */

	//bool pf_en = false;
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
