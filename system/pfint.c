#include <xinu.h>


// Global Variables
pid32 shared_heap_owner;
char* shared_heap_owner_start_addr;
char* shared_heap_my_start_addr;
char* shared_heap_size;


int is_shared_heap_access(uint32 vaddr){
  //kprintf("checking is shared by proc : %d\n",getpid());
	struct shared_mem *sh = proctab[getpid()].shared_heap_table->next;
	if(sh == NULL)
		return 0;

	while(sh  != NULL){	
	  /*	kprintf("------------------------------------------------------------------------\n");
		kprintf("Vaddr : %X\n", vaddr);

		kprintf(" Ownwer: %d\n",sh->shared_heap_owner);
		kprintf(" owner start addr: %X\n",sh->shared_heap_owner_start_addr);
		kprintf(" my start addr: %X\n",sh->shared_heap_my_start_addr);
		kprintf(" shared heap size: %X\n",sh->shared_heap_size);
	  */
		if(	
			(vaddr >= (uint32)sh->shared_heap_my_start_addr) && 
			(vaddr < ((uint32)sh->shared_heap_my_start_addr + (uint32) sh->shared_heap_size))
		){
		  //		kprintf("It is shared after all!!\n ");
			shared_heap_owner			 = sh->shared_heap_owner;	
			shared_heap_owner_start_addr = sh->shared_heap_owner_start_addr;
			shared_heap_my_start_addr    = sh->shared_heap_my_start_addr;
			shared_heap_size             = sh->shared_heap_size;
	
			return 1;
		}
		sh = sh->next;
		/*kprintf("***************************************************************************\n");
		kprintf("Vaddr : %X\n", vaddr);

		kprintf(" Ownwer: %d\n",sh->shared_heap_owner);
		kprintf(" owner start addr: %X\n",sh->shared_heap_owner_start_addr);
		kprintf(" my start addr: %X\n",sh->shared_heap_my_start_addr);
		kprintf(" shared heap size: %d\n",sh->shared_heap_size);*/
	}		

	return 0;

}

int pfint(){

	status_t status;
	int is_shared;
	uint32 new_frame,disk_frame,frame_translated;
	uint32 vaddr_owner_translated; 

	uint32 pd_bits_translated;
	uint32 pt_bits_translated;
	uint32 offset_translated;
	uint32 vpno_translated;

	//	kprintf("Page fault caused by proc : %d\n",getpid());

	uint32 cr2_word = read_cr2();
	//	kprintf("Faulted address : %X\n",cr2_word);

	// Dis-assemble the faulted address into pd, pt_bits and offset

	uint32 pd_bits = get_pd_bits(cr2_word);	
	uint32 pt_bits = get_pt_bits(cr2_word);	
	uint32 offset = get_offset_bits(cr2_word);	
	uint32 vpno	 = get_vpno(cr2_word);
	
	// Get the base address for the page directory for this process
	uint32 pdbr = proctab[getpid()].pdbr;
	pdbr = (pdbr>>12 )<< 12;
	
	uint32 owner_pdbr;	

	// So before we proceed, let us check if the address
	// faulted on has actually been allocated for this process
	// For this I need to access the free list of the process
	// and check if the address maps to any free block in the
	// list and if so, it is an illegal access.

	struct vmemblk* vmemlist = proctab[getpid()].vmemlist;
	struct vmemblk *curr = vmemlist->mnext;

	if(cr2_word > 8192*PAGE_SIZE + proctab[getpid()].vheap_size * PAGE_SIZE){
		kprintf("Segmentation Fault1\n");
		kill(getpid());
		return SYSERR;	
	}
	
	while(curr != NULL){
	  //	kprintf("curr addr : %X\t curr len : %X\n",curr->maddr, curr->mlength);
		if(curr->maddr <= cr2_word && (curr->maddr + curr->mlength) > cr2_word){
			kprintf("Segmentation Fault2\n");
			kill(getpid());
			return SYSERR;	
		}
		curr = curr->mnext;
	}

	// Check if the PD entry for this address exists
		
	pd_t *pde = (pd_t*)(pdbr);
	if((pde+pd_bits)->pd_pres == 0){
		// PD entry doesnt even exist
		// so add a pd entry, but before that 
		// create a PT for the new addr

		uint32 frame_pt = get_frame_for_PDPT(1);
		pt_t *pt = (pt_t*)(frame_pt * PAGE_SIZE);
	
		status = add_directory_entry(pde, frame_pt, pd_bits);


		// Before going ahead and allocating a new frame for the page, let us see 
		// if the access is to a shared heap region or not.


		is_shared = is_shared_heap_access(cr2_word);

		if(!is_shared){
			new_frame = get_frame_for_heap(vpno);
			if(new_frame == -1)
				return SYSERR;
			//kprintf("Escapong from here!!\n");
			map_page(pt, new_frame);
		} else {

		  //kprintf("Shared!!\n")	;
	
			if(shared_heap_owner == getpid()){	
				new_frame = get_frame_for_heap(vpno);
				if(new_frame == -1)
					return SYSERR;

				map_page(pt, new_frame);
			} else {
				// Translate from this process' virtual address space to 
				// the owner process' virtual address space

				uint32 my_start_vaddr 		= (uint32) shared_heap_my_start_addr;
				uint32 owner_start_vaddr 	= (uint32) shared_heap_owner_start_addr;

				vaddr_owner_translated = ( my_start_vaddr < owner_start_vaddr)?
												cr2_word + (owner_start_vaddr - my_start_vaddr):		
												cr2_word - (my_start_vaddr - owner_start_vaddr);	

				//kprintf("Translated addr in owner's space : %X\n",vaddr_owner_translated);
	
				// With this as your address now, try and see if there is a valid map

				pd_bits_translated = get_pd_bits(vaddr_owner_translated);		
				pt_bits_translated = get_pt_bits(vaddr_owner_translated);
				owner_pdbr = proctab[shared_heap_owner].pdbr;
				owner_pdbr = (owner_pdbr >>12) <<12;
		
				pd_t* pde_trans = (pd_t*)(owner_pdbr) + pd_bits_translated;
				pt_t* pte_trans = (pt_t*)(pde_trans->pd_base << 12) + pt_bits_translated;
				//kprintf("pde_trans : %X\t pte_trans : %X\n",*pde_trans, *pte_trans);

				pt_t* pte_mine = (pt_t*)((pde+pd_bits)->pd_base << 12) + pt_bits;
		
				if(pte_trans->pt_pres){
					frame_translated = pte_trans->pt_base;
					//kprintf("The owner's page was mapped to %d\n",frame_translated);

					map_page(pte_mine,frame_translated);

				} else {
					new_frame = get_frame_for_heap(vpno);
					if(new_frame == -1)
						return SYSERR;

					map_page(pt, new_frame);	
				}
			}
		}
	}	
	else{

		pt_t *pt = (pt_t*)(((pde+pd_bits)->pd_base << 12)) + pt_bits;

//		assert(pt->pt_pres == 0);
		//kprintf("dirty : %d\t pt_pres : %d\n",pt->pt_dirty, pt->pt_pres);	
		if(pt->pt_dirty == 1 && pt->pt_pres == 0) {
//			kprintf("************************************	YO	************************************\n");
			// Meaning the page was previously mapped and now has been swapped out to disk
			disk_frame  = find_frame_from_disk(vpno);
			if(disk_frame == -1){			
			  //kprintf("Something has gone wrong!\n");
				return SYSERR;
			}

			new_frame = get_frame_for_heap(vpno);
//			kprintf("new frame dirty : %d\n",new_frame);

			// copy from disk
			bcopy((disk_frame << 12), (new_frame << 12), PAGE_SIZE);
			map_page(pt, new_frame);

			return OK;	

		}

		// Before going ahead and allocating a new frame for the page, let us see 
		// if the access is to a shared heap region or not.
		is_shared = is_shared_heap_access(cr2_word);

		if(!is_shared){
			new_frame = get_frame_for_heap(vpno);
			if(new_frame == -1)
				return SYSERR;

			map_page(pt, new_frame);
		} else {

			if(shared_heap_owner == getpid()){	
				new_frame = get_frame_for_heap(vpno);
				if(new_frame == -1)
					return SYSERR;

				map_page(pt, new_frame);
			} else {
				// Translate from this process' virtual address space to 
				// the owner process' virtual address space

				uint32 my_start_vaddr 		= (uint32) shared_heap_my_start_addr;
				uint32 owner_start_vaddr 	= (uint32) shared_heap_owner_start_addr;

				vaddr_owner_translated = ( my_start_vaddr < owner_start_vaddr)?
												cr2_word + (owner_start_vaddr - my_start_vaddr):		
												cr2_word - (my_start_vaddr - owner_start_vaddr);	

				//kprintf("Translated addr in owner's space : %X\n",vaddr_owner_translated);
	
				// With this as your address now, try and see if there is a valid map

				pd_bits_translated = get_pd_bits(vaddr_owner_translated);		
				pt_bits_translated = get_pt_bits(vaddr_owner_translated);
				owner_pdbr = proctab[shared_heap_owner].pdbr;
				owner_pdbr = (owner_pdbr >>12) <<12;
		
				pd_t* pde_trans = (pd_t*)(owner_pdbr) + pd_bits_translated;
				pt_t* pte_trans = (pt_t*)(pde_trans->pd_base << 12) + pt_bits_translated;
				//kprintf("pde_trans : %X\t pte_trans : %X\n",*pde_trans, *pte_trans);

				pt_t* pte_mine = (pt_t*)((pde+pd_bits)->pd_base << 12) + pt_bits;
		
				if(pte_trans->pt_pres){
					frame_translated = pte_trans->pt_base;
					//			kprintf("The ownerR's page was mapped to %d\n",frame_translated);

					map_page(pte_mine,frame_translated);

				} else {
					new_frame = get_frame_for_heap(vpno);
					if(new_frame == -1)
						return SYSERR;

					map_page(pt, new_frame);	
				}
			}
		}
	}
	
	pt_t *pt_new = (pt_t*)((pde+pd_bits)->pd_base << 12);
//	kprintf("new pt addr: %X\t and val : %X\n",pt_new, *(pt_new))	;	
		
	return OK;

}
