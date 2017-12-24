#include <xinu.h>

fr_map_t frame_tab[NFRAMES];
fr_map_t ffs_tab[FFS_SIZE];
fr_map_t disk_tab[DISK_SIZE];

uint32 num_free_ffs_frames = FFS_SIZE;

/* The memory layout for the new xinu will be something like this :

   	   0 - 1024 frames ---------> Kernel managed
	1024 - 1043 frames ---------> For process PDs and PTs
	1044 - 2047 frames ---------> For process pages
	2048 - 4096 frames ---------> For process heaps and stacks
	4096 - 8192 frames ---------> Disk emulation space

*/

syscall frame_tab_init(){

	int i;
	
	// Note that the frames under our purview only
	// from 1024 to 2047. 
	
	// TODO: Try relaxing the 1024 constraint

	for(i=0; i< NFRAMES; i++){

		frame_tab[i].fr_status 	= FR_UNMAPPED;
		frame_tab[i].fr_pid 	= 0;	
		frame_tab[i].fr_vpno	= 0;
		frame_tab[i].fr_refcnt	= 0;
		frame_tab[i].fr_type 	= 0;
		frame_tab[i].fr_dirty	= 0;
	}
	
	for(i=0; i< FFS_SIZE; i++){

		ffs_tab[i].fr_status 	= FR_UNMAPPED;
		ffs_tab[i].fr_pid 		= 0;	
		ffs_tab[i].fr_vpno		= 0;
		ffs_tab[i].fr_refcnt	= 0;
		ffs_tab[i].fr_type 		= 0;
		ffs_tab[i].fr_dirty		= 0;
	}

	for(i=0; i< DISK_SIZE; i++){

		ffs_tab[i].fr_status 	= FR_UNMAPPED;
		ffs_tab[i].fr_pid 		= 0;	
		ffs_tab[i].fr_vpno		= 0;
		ffs_tab[i].fr_refcnt	= 0;
		ffs_tab[i].fr_type 		= 0;
		ffs_tab[i].fr_dirty		= 0;
	}

	return OK;
}

uint32 get_frame_for_PDPT(int type){

	int frame_no = 0;

	// Sub-optimal way to find the available free frame. 
	// I am following a first fit allocation here.

	char *ty;
	ty = (type==0)?"Directory":"Table";

	while(frame_no < NFRAMES){
		if(frame_tab[frame_no].fr_status == FR_UNMAPPED){
		//	kprintf("Frame no : %d allocated for page %s\n",frame_no+1024, ty);
			frame_tab[frame_no].fr_status 	= FR_MAPPED;
			frame_tab[frame_no].fr_pid		= getpid();		 	
			frame_tab[frame_no].fr_vpno		= 0;
			frame_tab[frame_no].fr_refcnt	= 1;
			frame_tab[frame_no].fr_type 	= (!type)?PAGE_DIR_TYPE:PAGE_TBL_TYPE;
			frame_tab[frame_no].fr_dirty	= 0;
			break;
		} else {
			frame_no++;
		}
	}

	return frame_no+FRAME_BEGIN;
}


uint32 get_frame_from_disk(vpn){

	uint32 frame_no = 0;

	while(frame_no < DISK_SIZE){
		if(disk_tab[frame_no].fr_status == FR_UNMAPPED){
//			kprintf("Disk Frame no : %d allocated with vpn : %X\n",frame_no+4096, vpn);
			disk_tab[frame_no].fr_status 	= FR_MAPPED;
			disk_tab[frame_no].fr_pid		= getpid();		 	
			disk_tab[frame_no].fr_vpno		= vpn;
			disk_tab[frame_no].fr_refcnt	= 1;
			disk_tab[frame_no].fr_type 		= PAGE_TYPE; 
			disk_tab[frame_no].fr_dirty		= 0;	
			break;
		} else {
			frame_no++;
		}
	}
	
	if(frame_no != DISK_SIZE)
		return DISK_START + frame_no;	
	else
		return SYSERR;
}


uint32 get_frame_for_heap(uint32 vpn){

	int frame_no = 0;
	uint32 pd_bits;
	uint32 pt_bits;
	uint32 offset;	

	// Sub-optimal way to find the available free frame. 
	// I am following a first fit allocation here.

	while(frame_no < FFS_SIZE){
		if(ffs_tab[frame_no].fr_status == FR_UNMAPPED){
		//	kprintf("Frame no : %d allocated for page %s\n",frame_no+1024, ty);
			ffs_tab[frame_no].fr_status = FR_MAPPED;
			ffs_tab[frame_no].fr_pid	= getpid();		 	
			ffs_tab[frame_no].fr_vpno	= vpn;
			ffs_tab[frame_no].fr_refcnt	= 1;
			ffs_tab[frame_no].fr_type 	= PAGE_TYPE; 
			ffs_tab[frame_no].fr_dirty	= 0;	
			num_free_ffs_frames--;
			break;	
		} else {
			frame_no++;
		}
	}
	if(frame_no < FFS_SIZE){
//		kprintf("Alloting FFS Frame no : %d\n",frame_no+2048);
		return frame_no+FFS_START;
	} else{
		frame_no=0;
		// We have a problem here.. we done have any free frames to allocate
		// time to evict a page to the disk based on eviction policy

		//kprintf("Have to evict now!!\n");
	
		// Eviction Policy : Approx LRU (GCM)	
		// Here for each frane that I visit, I need to check if the pt_acc 
		// bit is unset by the process currently using that frame.	
	
		uint32 pdbr = proctab[getpid()].pdbr;
		uint32 vpno; 

		while(frame_no < FFS_SIZE){

			vpno = ffs_tab[frame_no].fr_vpno;

			pd_bits = (vpno >>10);
			pt_bits = (vpno << 22) >> 22;
			
//			kprintf("pd_bits : %X\t pd_bits : %X\t and vpn : %X\n",pd_bits, pt_bits, vpno);
			pd_t *pde = (pd_t*)(pdbr) + pd_bits;
			pt_t *pte = (pt_t*)(pde->pd_base * PAGE_SIZE)+pt_bits;
			
//			kprintf("pde : %X\t pte : %X\n",*pde, *pte);

//		kprintf("vpn : %X\t pd_bits : %X\t pt_bits : %X\n ",vpn, pd_bits, pt_bits);

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
						kill(getpid());	
					}

				//	kprintf("disk frame for proc : %d is : %d\n",getpid(), disk_frame);
					
					// write to disk	
					bcopy(((frame_no+FFS_START) << 12), (disk_frame << 12), PAGE_SIZE);

				} 

				// Set pt_pres = 0;
				pte->pt_pres = 0;

				//kprintf("Returning %d\n", frame_no+FFS_START);
				return frame_no + FFS_START;	
			}	
			
			frame_no++;

			if(frame_no == FFS_SIZE)
				frame_no = 0;
		}
	}		
}


syscall initialize_page_dir(pd_t *proc_pd){

	int i;
	for( i =0 ; i < NUM_PAGE_ENTRIES; i++){		
  		proc_pd->pd_pres	= 0;
  		proc_pd->pd_base	= 0;
			
		proc_pd++;
	}
	return OK;
}


syscall add_directory_entry(pd_t *proc_pd, uint32 frame_pt, int pos){

	int i;
//	kprintf("proc_pd : %X\t pos: %d\t for proc : %d\n",proc_pd,pos, getpid());

  	(proc_pd+pos)->pd_pres		= 1;
  	(proc_pd+pos)->pd_write 	= 1;
  	(proc_pd+pos)->pd_user		= 0;
  	(proc_pd+pos)->pd_pwt		= 0;
  	(proc_pd+pos)->pd_pcd		= 0;
  	(proc_pd+pos)->pd_acc		= 0;
  	(proc_pd+pos)->pd_mbz		= 0;
  	(proc_pd+pos)->pd_fmb		= 0;
  	(proc_pd+pos)->pd_global	= 0;
  	(proc_pd+pos)->pd_avail 	= 0;
	
	// Point the first entry to the Page table right after
  	(proc_pd+pos)->pd_base		= frame_pt;	 
//	kprintf("pd : %X\t frame_pt : %X\n",proc_pd+pos, *(proc_pd+pos));

	return OK; 
}

syscall initialize_page_table(pt_t *proc_pt, int j){

	int i;	
	for(i=0 ; i< NUM_PAGE_ENTRIES ; i++){
		proc_pt->pt_pres	= 1;
		proc_pt->pt_write 	= 1;
		proc_pt->pt_user	= 0;
		proc_pt->pt_pwt		= 0;
		proc_pt->pt_pcd		= 0;
		proc_pt->pt_acc		= 0;
		proc_pt->pt_dirty 	= 0;
		proc_pt->pt_mbz		= 0;
		proc_pt->pt_global	= 0;
		proc_pt->pt_avail 	= 0;
		proc_pt->pt_base	= j*NUM_PAGE_ENTRIES + i;	

		proc_pt++;
	}
	return OK;

}

syscall map_page(pt_t *proc_pt, uint32 frame){
	
	proc_pt->pt_pres	= 1;
	proc_pt->pt_write 	= 1;
	proc_pt->pt_user	= 0;
	proc_pt->pt_pwt		= 0;
	proc_pt->pt_pcd		= 0;
	proc_pt->pt_acc		= 0;
	proc_pt->pt_dirty 	= 0;
	proc_pt->pt_mbz		= 0;
	proc_pt->pt_global	= 0;
	proc_pt->pt_avail 	= 0;
	proc_pt->pt_base	= frame;	

	//kprintf("In map : %X\t and frame : %X\n", proc_pt, proc_pt->pt_base);

//	kprintf("In map : %X\t and frame : %X\n", proc_pt,frame);

	return OK;
}

uint32 find_frame_from_disk(uint32 vpno){

	int i;
	for(i=0;i<DISK_SIZE; i++){
//		kprintf("Stored vpn : %X\t given VPN : %X\n",disk_tab[i].fr_vpno, vpno);
		if(disk_tab[i].fr_vpno == vpno && disk_tab[i].fr_pid == getpid())
			return DISK_START+i;
	}

	return SYSERR;
}


