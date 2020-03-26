/*
 * SAWP
 */

#include "interface.hh"


	int window = 1;						// window size
	int threshold = 1;					// minimum amount of tag_count for it to be a succesful prefetch, threshold is N - 1, but 1 if N is 1
	int locked = 0;						// is 1 if the window is locked from increasing. Prevents the window size from oscillating
	int remaining_accesses = 0;			// the number of remaining accesses in the current window before next prefetching starts
	int tag_count = 0;					// the amount of requested blocks that were prefetched to calculate the accuracy
	//int max_window = 4;
	
void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

    DPRINTF(HWPrefetch, "Initialized Sequential Adaptive Window Prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
    /* Increment the current tag count then clear the tag */
    Addr curr_addr = stat.mem_addr;
	if(!stat.miss){
		if (get_prefetch_bit(curr_addr)){
			tag_count++;
			clear_prefetch_bit(curr_addr);
		} 	

	}
	
	
	/*int tag_count = 0;
	for (int i = 0; i < W; i++){
        Addr addr1 = curr_addr + i * BLOCK_SIZE;
        stat.mem_addr = addr1;
		if (!stat.miss){
			if(get_prefetch_bit){tag_count++;}
		}
	}*/
	
	
    /*
     * In case of a new prefetching sequence:
	 * - Update window and threshold.
	 * - Issue a prefetch request for all blocks in the sequence not already in cache.
	 * - Tag all blocks in the sequence.
     */
	 
	if (remaining_accesses <= 0){
		if (tag_count < threshold && window >= 2){		// low accuracy, decrease window and threshold
			window--;
			locked = 1;
			if (window >= 3){
				threshold = window - 1;
			}
			else {
				threshold = 1;
			}
		}
		else if(locked)	{								// use locked to prevent window size from increasing right after it has decreased
			locked = 0;
		}	
		//else if(window < max_window)
		else{
			// check for upper window size?
			window++;
			threshold = window - 1;
			//locked = 1;
		}
		
		tag_count = 0; 									// clear the tag count before the new sequence
		remaining_accesses = window;					// set remaining_accesses
		Addr pf_addr = curr_addr + BLOCK_SIZE;
		for (int i = 1; i <= window; i++){
			pf_addr = curr_addr + i * BLOCK_SIZE;
			if( pf_addr >= 0 && pf_addr <= MAX_PHYS_MEM_ADDR){
				if (!in_cache(pf_addr)) {
					issue_prefetch(pf_addr);
				}
			}
			set_prefetch_bit(curr_addr + i * BLOCK_SIZE);
		}
	}
	 else{
		remaining_accesses--;							// decrement remaining_accesses
	}
	 
}

void prefetch_complete(Addr addr) {
    
	/*
     * Called when a block requested by the prefetcher has been loaded.
     */
	 
}
