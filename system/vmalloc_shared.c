#include <xinu.h>

char *vmalloc_shared(pid32 pid, uint32 nbytes, char *ptr){

	int i;
	struct vmemblk *prev, *curr, *leftover;
	intmask mask;
	status_t status;

	char *vptr = (char*)NULL;	

	mask = disable();
	
	if(nbytes == 0){
		kprintf("Out of memory\n");
		restore(mask);
		return SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	
	//	kprintf("Vmalloc_shared called with owner pid : %d\t nbytes : %d\t by proc : %d\n",pid, nbytes, getpid());

	if(nbytes > proctab[getpid()].vmemlist->mlength){
		kprintf("Out of Memory\n");
		return SYSERR;
	}

	prev = (proctab[getpid()].vmemlist);
	curr = proctab[getpid()].vmemlist->mnext;
		
	while(curr != NULL){

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			proctab[getpid()].vmemlist->mlength -= nbytes;
			vptr = (char *)(curr->maddr);
			break;

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = getmem(sizeof(struct vmemblk));
			leftover->maddr = (curr->maddr + nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			proctab[getpid()].vmemlist->mlength -= nbytes;
			vptr = (char *)(curr->maddr);
			break;
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	
	struct shared_mem *sh_mem_tbl = getmem(sizeof(struct shared_mem)); 
	
	sh_mem_tbl->shared_heap_owner = pid;
	if(ptr == NULL){
		sh_mem_tbl->shared_heap_owner_start_addr = vptr;
		sh_mem_tbl->shared_heap_my_start_addr = vptr;
	} else{
		sh_mem_tbl->shared_heap_my_start_addr = vptr;
		sh_mem_tbl->shared_heap_owner_start_addr = ptr;
	}
	sh_mem_tbl->shared_heap_size = nbytes;
	
	if(ptr == NULL){
		// We are not the owner for the requested shared addr space
		sh_mem_tbl->is_owner = 1; 		
		for(i=0;i<NPROC;i++)
			sh_mem_tbl->sharers_list[i] = 0;	
	}

	sh_mem_tbl->next = NULL;
	
	struct shared_mem *head = proctab[getpid()].shared_heap_table;
	if(head -> next == NULL){
		head->next = sh_mem_tbl;
	}
	else{
		while(head->next != NULL)
			head = head->next;
		head->next = sh_mem_tbl;
	}

	if(ptr != NULL){
		//Update owner's sharer list
		head = proctab[pid].shared_heap_table->next;
		while(head!= NULL){
			if(head->is_owner == 1 && head->shared_heap_owner_start_addr == ptr){
				head->sharers_list[getpid()] = 1;
			}
			head = head->next;
		}
	}

	restore(mask);

	if(vptr == (char*)NULL)
		return (char *)SYSERR;
		
	return vptr;
}
