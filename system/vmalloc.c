#include <xinu.h>

char *vmalloc(uint32 nbytes){

	struct vmemblk *prev, *curr, *leftover;
	intmask mask;

	mask = disable();
	
	if(nbytes == 0){
		kprintf("Out of memory\n");
		restore(mask);
		return SYSERR;
	}

//	kprintf("------------------------------------------------------------------\n")	;
	nbytes = (uint32) roundmb(nbytes);
//	kprintf("nbytes : %d\n",nbytes);

	if(nbytes > proctab[getpid()].vmemlist->mlength){
		kprintf("Out of Memory\n");
		return SYSERR;
	}

	prev = (proctab[getpid()].vmemlist);
//	kprintf("prev : %d\n",(uint32)prev);

	//if(proctab[getpid()].vmemlist->mnext == NULL){
//	//	kprintf("init null case !! \n");
	//	proctab[getpid()].vmemlist->mnext = (struct memblk*)(2048*PAGE_SIZE);
//	//	proctab[getpid()].vmemlist->mlength = proctab[getpid()].vheap_size* PAGE_SIZE;

	//	proctab[getpid()].vmemlist->mnext->mnext = (struct memblk*)NULL;
	//	proctab[getpid()].vmemlist->mnext->mlength = proctab[getpid()].vheap_size* PAGE_SIZE;
	//
	//}

	curr = proctab[getpid()].vmemlist->mnext;
//	kprintf("first block: %d\n",(uint32)curr);
//	kprintf("curr len: %d\n",curr->mlength);
		
	while(curr != NULL){

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			proctab[getpid()].vmemlist->mlength -= nbytes;
//			kprintf("left len : %u\n",proctab[getpid()].vmemlist->mlength);
			restore(mask);
			return (char *)(curr->maddr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = getmem(sizeof(struct vmemblk));
			leftover->maddr = (curr->maddr + nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			proctab[getpid()].vmemlist->mlength -= nbytes;
//			kprintf("left len : %u\n",proctab[getpid()].vmemlist->mlength);
			restore(mask);
			return (char *)(curr->maddr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	restore(mask);
	return (char *)SYSERR;
}
