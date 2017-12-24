#include <xinu.h>

void swap_daemon(){

	uint32 frame_no=0;
	uint32 pd_bits;
	uint32 pt_bits;
	uint32 offset;	

	uint32 pdbr = proctab[getpid()].pdbr;
	uint32 vpno; 
	intmask mask;

	while(1){
		mask = disable();	
		if(num_free_ffs_frames < LW){
		  //kprintf("---------------------------------------------------------------------------------------\n");
		  //kprintf("				   Swap Daemon Swapping								\n");	
		  //kprintf("---------------------------------------------------------------------------------------\n");
			while(num_free_ffs_frames <= HW){
				while(frame_no < FFS_SIZE){

					vpno = ffs_tab[frame_no].fr_vpno;

					pd_bits = (vpno >>10);
					pt_bits = (vpno << 22) >> 22;
					
					//			kprintf("pd_bits : %X\t pd_bits : %X\t and vpn : %X\n",pd_bits, pt_bits, vpno);
					pd_t *pde = (pd_t*)(pdbr) + pd_bits;
					pt_t *pte = (pt_t*)(pde->pd_base * PAGE_SIZE)+pt_bits;

					// Approx LRU	

					if(pte->pt_acc == 1){
						pte->pt_acc = 0; // Unset the access bit if 1;
					} else {
						// We found our victim page;		
						// Check if dirty so that we can write back!!
						if(pte->pt_dirty){
							// Have to write back to disk
							//write_back(frame_no);

							uint32 disk_frame = get_frame_from_disk(vpno);
							if(disk_frame == -1){
							  //kprintf("Disk out of Space!!\n");
							}
							//		kprintf("disk frame for proc : %d is : %d\n",getpid(), disk_frame);
							
							// write to disk	
							bcopy(((frame_no+FFS_START) << 12), (disk_frame << 12), PAGE_SIZE);

						} 
						// Set pt_pres = 0;
						pte->pt_pres = 0;
						num_free_ffs_frames++;
						break;
					
					}	
					
					frame_no++;

					if(frame_no == FFS_SIZE)
						frame_no = 0;
				}
			}
			//			kprintf("iAfter,... num free ffs frames : %d\n",num_free_ffs_frames);
		}
		enable();
		sleepms(SD_SLEEP_TIME);
	}
}

