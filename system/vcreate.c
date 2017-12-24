#include <xinu.h>

pid32	vcreate(
	  void		*funcaddr,	/* Address of the function	*/
	  uint32	ssize,		/* Stack size in bytes		*/
	  uint32	hsize,		/* Heap size in bytes		*/
	  pri16		priority,	/* Process priority > 0		*/
	  char		*name,		/* Name (for debugging)		*/
	  uint32	nargs,		/* Number of args that follow	*/
	  long 		args
	){

  //	kprintf("called\n");
	int i;
	uint32 disk_frame;

	pid32 pid = create(funcaddr, ssize, priority, name, nargs, args);

	if(pid == SYSERR)
		return SYSERR;

	// sanity check on the requested heap size
	if(hsize > MAX_HEAP_SIZE){
		kprintf("cant support requested heap size!! Aborting.... \n");
		return SYSERR;
	}

	// When we initially create a process, any block above the 8192nd page
	// can ideally be used to allocate the heap space requested by the process
	// The 8192 page here comes to picture because I chose to map the entire
	// lower 32MB of a process's virtual address to the physical addres
	// so virtual pages 0-8191 are gone. :D

	proctab[pid].vheap_size = hsize;

	// Also to start off, all the pages belonging to the heap region of the
	// program are intially in the disk and brought to memory on demand.
	// So mapping the heap region to the disk.

	// for(i=0 ; i<hsize; i++){
	// 	
	// 	disk_frame = get_frame_from_disk();
 	// 	if(disk_frame == -1){
	// 		kprintf("Disk out of space! Killing process!");
	// 		kill(pid);
	// 		return SYSERR;
	// 	}
	// 	
	// 	map_page(VIRT_HEAP_START + i, disk_frame);

	// }
	
	// Initializing heap list	
	proctab[pid].shared_heap_table = getmem(sizeof(struct shared_mem)); 
	proctab[pid].shared_heap_table->next = NULL;	

	// Initializing the free list for the virtual address area
	proctab[pid].vmemlist = getmem(sizeof(struct vmemblk)); 
	proctab[pid].vmemlist->mlength = hsize * PAGE_SIZE;

	proctab[pid].vmemlist->mnext = getmem(sizeof(struct vmemblk)); 
	proctab[pid].vmemlist->mnext->mnext = NULL;
	proctab[pid].vmemlist->mnext->maddr = VIRT_HEAP_START* PAGE_SIZE;
	proctab[pid].vmemlist->mnext->mlength = hsize * PAGE_SIZE;




	return pid;
}

