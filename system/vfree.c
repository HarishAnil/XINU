/* vfreemem.c - vfreemem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  vfreemem  -  Free heap space 
 *------------------------------------------------------------------------
 */

int check_is_shared_heap_access(uint32 vaddr){
	struct shared_mem *sh = proctab[getpid()].shared_heap_table->next;
	if(sh == NULL)
		return 0;

	while(sh  != NULL){	
		if(	vaddr == (uint32)sh->shared_heap_my_start_addr){
			if(sh->is_owner){
				return 2;
			}	
			return 1;
		}	
		sh = sh->next;
	}		
	return 0;

}
syscall	vfree(
	  char		*blkaddr,	/* Pointer to memory block	*/
	  uint32	nbytes		/* Size of block in bytes	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	vmemblk	*next, *prev , *block;
	uint32	top;

	uint32 pd_bits;
	uint32 pt_bits;
	uint32 offset;	
	uint32 pdbr;
	int i;
	struct shared_mem *curr,*previ, *curr1, *previ1;
	
	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return SYSERR;
	}
	

	block = (struct vmemblk*) getmem(sizeof(struct vmemblk));  //c
	block->mnext = (struct vmemblk*)NULL;
	block->mlength = nbytes;
	block->maddr = (uint32)blkaddr;

   	struct vmemblk* vmemlist = proctab[currpid].vmemlist;
	prev = vmemlist;			/* parse the free list	*/
	next = vmemlist->mnext;

	while ((next != NULL) && (next->maddr < block->maddr)) {  
		prev = next;
		next = next->mnext;
		//kprintf("in it\n");
	}

	if (prev == vmemlist) {		/* Compute top of previous blkaddr*/
		top = (uint32) NULL;
		//kprintf("top 1\n");
	} else {
		top = (uint32) (prev->maddr) + prev->mlength; //changed
		//kprintf("top 2\n");
	}

	/* Ensure new blkaddr does not overlap previous or next blkaddrs	*/

	if (((prev != vmemlist) && (uint32) block < top)
	    || ((next != NULL)	&& (block->maddr +nbytes) >next->maddr)) {  
		restore(mask);
       	kprintf("SYSERR\n");
		return SYSERR;
	}


        //add total available bytes to head

	proctab[currpid].vmemlist->mlength += nbytes;

	/* Either coalesce with previous blkaddr or add to free list */

	if (top ==  block->maddr){
        prev->mlength += nbytes;
		block->maddr = prev->maddr; 
	} 

	else {       		
		block->mlength = nbytes;
		prev->mnext = block;
		block->mnext = next;
	}


	if ((block->maddr + block->mlength) == next->maddr) {  
	
		block->mlength = block->mlength +  next->mlength;
		block->mnext = next ->mnext; 
	
        }

	
	// Because the allocated memory is now freed, we need to remove the mapping 
	// of this page to its currently mapped frame.

	if(check_is_shared_heap_access((uint32)blkaddr) == 1){
			
		curr = proctab[getpid()].shared_heap_table->next; 	
		previ = proctab[getpid()].shared_heap_table; 	
		
		while(curr != NULL){	
			if(blkaddr == curr->shared_heap_my_start_addr){
				previ->next = curr->next;
				curr->next = NULL;
				break;	
			}else{
				previ = curr;
				curr = curr->next;
			}
		}
	} else if(check_is_shared_heap_access((uint32)blkaddr) == 2){
	  //kprintf("came off here!!\n");
		curr = proctab[getpid()].shared_heap_table->next; 	
		previ = proctab[getpid()].shared_heap_table; 	
		
		while(curr != NULL){	
			if(blkaddr == curr->shared_heap_my_start_addr){
				previ->next = curr->next;
				curr->next = NULL;
				break;	
			}else{
				previ = curr;
				curr = curr->next;
			}
		}


		for(i=0;i<NPROC;i++){
			if(curr->sharers_list[i] == 1){
				curr1 = proctab[i].shared_heap_table->next; 	
				previ1 = proctab[i].shared_heap_table; 	
				
				while(curr1 != NULL){	
					if(blkaddr == curr1->shared_heap_my_start_addr){
						previ1->next = curr1->next;
						curr1->next = NULL;
						break;	
					}else{
						previ1 = curr1;
						curr1 = curr1->next;
					}
				}
			
				pdbr = proctab[i].pdbr;	
				
				pd_bits = get_pd_bits((uint32)blkaddr);
				pt_bits = get_pt_bits((uint32)blkaddr);

				pd_t *pde = (pd_t*)(pdbr);
				pt_t *pte = (pt_t*)((pde+pd_bits)->pd_base << 12) + pt_bits;

				pte->pt_pres = 0;			
	
			}
		}
	}

	pdbr = proctab[getpid()].pdbr;	
	
	pd_bits = get_pd_bits((uint32)blkaddr);
	pt_bits = get_pt_bits((uint32)blkaddr);

	pd_t *pde = (pd_t*)(pdbr);
	pt_t *pte = (pt_t*)((pde+pd_bits)->pd_base << 12) + pt_bits;

	pte->pt_pres = 0;

	//kprintf("free done!\n");
	restore(mask);
	return OK;

}






