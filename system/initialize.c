/* initialize.c - nulluser, sysinit */

/* Handle system initialization and become the null process */

#include <xinu.h>
#include <string.h>

extern	void	start(void);	/* Start of Xinu code			*/
extern	void	*_end;		/* End of Xinu code			*/

/* Function prototypes */

extern	void main(void);	/* Main is the first process created	*/
static	void sysinit(); 	/* Internal system initialization	*/
extern	void meminit(void);	/* Initializes the free memory list	*/
local	process startup(void);	/* Process to finish startup tasks	*/

/* Declarations of major kernel variables */

struct	procent	proctab[NPROC];	/* Process table			*/
struct	sentry	semtab[NSEM];	/* Semaphore table			*/
struct	memblk	memlist;	/* List of free memory blocks		*/

/* Active system status */

int	prcount;		/* Total number of live processes	*/
pid32	currpid;		/* ID of currently executing process	*/

/* Control sequence to reset the console colors and cusor positiion	*/

#define	CONSOLE_RESET	" \033[0m\033[2J\033[;H"

/*------------------------------------------------------------------------
 * nulluser - initialize the system and become the null process
 *
 * Note: execution begins here after the C run-time environment has been
 * established.  Interrupts are initially DISABLED, and must eventually
 * be enabled explicitly.  The code turns itself into the null process
 * after initialization.  Because it must always remain ready to execute,
 * the null process cannot execute code that might cause it to be
 * suspended, wait for a semaphore, put to sleep, or exit.  In
 * particular, the code must not perform I/O except for polled versions
 * such as kprintf.
 *------------------------------------------------------------------------
 */

void	nulluser()
{	
	struct	memblk	*memptr;	/* Ptr to memory block		*/
	uint32	free_mem;		/* Total amount of free memory	*/
	status_t status;
	
	/* Initialize the system */

	sysinit();

	/* Output Xinu memory layout */
	free_mem = 0;
	for (memptr = memlist.mnext; memptr != NULL;
						memptr = memptr->mnext) {
		free_mem += memptr->mlength;
	}
	kprintf("%10d bytes of free memory.  Free list:\n", free_mem);
	for (memptr=memlist.mnext; memptr!=NULL;memptr = memptr->mnext) {
	    kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)memptr, ((uint32)memptr) + memptr->mlength - 1);
	}

	kprintf("%10d bytes of Xinu code.\n",
		(uint32)&etext - (uint32)&text);
	kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)&text, (uint32)&etext - 1);
	kprintf("%10d bytes of data.\n",
		(uint32)&ebss - (uint32)&data);
	kprintf("           [0x%08X to 0x%08X]\n\n",
		(uint32)&data, (uint32)&ebss - 1);

	/* Enable interrupts */

	enable();

	/* Initialize the network stack and start processes */

	net_init();
	
	
	/* Before proceeding further let us start paging and setup stuff needed for that before doing so */
	
	int i;
	uint32 frame_pd, frame_pt;
	pt_t *nullproc_pt;

	// First initialise the free-list for our frame fetcher
	frame_tab_init();

	/* PAGE DIRECORY */

	// Get a free frame to create the PD
	frame_pd = get_frame_for_PDPT(0);
	
	// Create a Page directory for the null process
	pd_t *nullproc_pd = (pd_t*)(frame_pd * PAGE_SIZE);	


	// initialize the page directory
	status = initialize_page_dir(nullproc_pd);
	if(status == SYSERR)
		return SYSERR;

	// Map the page directory

	// Initially mapping the entire physical frame 
	// onto the virtual addr space of the process

	// What I am currently doing here is to map the entire physical frames
	// from 0 to 8192 into this page table as a one-to-one map!! I know it is wasteful

	// Now i am trying to map specifically!!

	for(i=0; i<4; i++){
		
		/* PAGE TABLE*/

		// Get a free frame to create the PT
		frame_pt = get_frame_for_PDPT(1);
		
		// Create a Page directory handle for the null process
		nullproc_pt = (pt_t*)(frame_pt * PAGE_SIZE);	
	
		status = add_directory_entry(nullproc_pd, frame_pt, i);

		// Initialize the page table to map to the static regions

		status = initialize_page_table(nullproc_pt,i);
		if(status == SYSERR)
			return SYSERR;


	}

	// Also map stack 

	uint32 stk_ptr = (uint32)proctab[0].prstkptr;
	//kprintf("Stack ptr: %X\n",stk_ptr)	;
	
	uint32 dir_entry = (uint32)proctab[currpid].prstkbase >>22;
	//kprintf("Stack dir : %X\n",dir_entry)	;

	uint32 page_no = (((uint32)proctab[currpid].prstkbase >>12)<<22)>>22;
	//kprintf("Stack page no: %X\n",page_no)	;

//	status = add_directory_entry(nullproc_pd, )
		

	
//	kprintf("value of proc_pd : %X\n",*nullproc_pd);


	// set the pdbr so that hardware knows where to look for the dir
	unsigned long cr3_word = frame_pd* PAGE_SIZE;
	cr3_word = (cr3_word >> 12) << 12;	
	write_cr3(cr3_word);
	
//	kprintf("Cr3 value written : 0x%X", read_cr3());

	// save into proctab
	proctab[getpid()].pdbr = cr3_word; 

	// Register our interrupt service routine
	set_evec(14, (unsigned long)pfintr);

	pd_t *a = (pd_t*)(read_cr3());
	pt_t *b = (pt_t*)(((a+1)->pd_base)*PAGE_SIZE);
//	kprintf("addr of PD : %X\n",*a);
//	kprintf("addr of PT : %X\n",*b);
	unsigned long adr = read_cr2();
//	kprintf("addr in Cr2 : %X\n",adr);
	
	// enable paging
	enable_paging();

	/* Create a process to finish startup and start main */

	resume(create((void *)startup, INITSTK, INITPRIO,
					"Startup process", 0, NULL));

	/* Become the Null process (i.e., guarantee that the CPU has	*/
	/*  something to run when no other process is ready to execute)	*/

	while (TRUE) {
		;		/* Do nothing */
	}

}


/*------------------------------------------------------------------------
 *
 * startup  -  Finish startup takss that cannot be run from the Null
 *		  process and then create and resume the main process
 *
 *------------------------------------------------------------------------
 */
local process	startup(void)
{
	uint32	ipaddr;			/* Computer's IP address	*/
	char	str[128];		/* String used to format output	*/


	/* Use DHCP to obtain an IP address and format it */

	ipaddr = getlocalip();
	if ((int32)ipaddr == SYSERR) {
		kprintf("Cannot obtain an IP address\n");
	} else {
		/* Print the IP in dotted decimal and hex */
		ipaddr = NetData.ipucast;
		sprintf(str, "%d.%d.%d.%d",
			(ipaddr>>24)&0xff, (ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,        ipaddr&0xff);
	
		kprintf("Obtained IP address  %s   (0x%08x)\n", str,
								ipaddr);
	}

	/* Create a process to execute function main() */

	resume(create((void *)main, INITSTK, INITPRIO,
					"Main process", 0, NULL));

	/* Startup process exits at this point */

	return OK;
}


/*------------------------------------------------------------------------
 *
 * sysinit  -  Initialize all Xinu data structures and devices
 *
 *------------------------------------------------------------------------
 */
static	void	sysinit()
{
	int32	i;
	struct	procent	*prptr;		/* Ptr to process table entry	*/
	struct	sentry	*semptr;	/* Ptr to semaphore table entry	*/

	/* Reset the console */

	kprintf(CONSOLE_RESET);
	kprintf("\n%s\n\n", VERSION);

	/* Initialize the interrupt vectors */

	initevec();
	
	/* Initialize free memory list */
	
	meminit();
	stacktrace();
	
	/* Initialize system variables */

	/* Count the Null process as the first process in the system */

	prcount = 1;

	/* Scheduling is not currently blocked */

	Defer.ndefers = 0;

	/* Initialize process table entries free */

	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		prptr->prstate = PR_FREE;
		prptr->prname[0] = NULLCH;
		prptr->prstkbase = NULL;
		prptr->prprio = 0;
	}

	/* Initialize the Null process entry */	

	prptr = &proctab[NULLPROC];
	prptr->prstate = PR_CURR;
	prptr->prprio = 0;
	strncpy(prptr->prname, "prnull", 7);
	prptr->prstkbase = getstk(NULLSTK);
	prptr->prstklen = NULLSTK;
	prptr->prstkptr = 0;
	//kprintf("Stack base : %X\n",prptr->prstkbase);
	currpid = NULLPROC;
	
	/* Initialize semaphores */

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		semptr->sstate = S_FREE;
		semptr->scount = 0;
		semptr->squeue = newqueue();
	}

	/* Initialize buffer pools */

	bufinit();

	/* Create a ready list for processes */

	readylist = newqueue();

	/* initialize the PCI bus */

	pci_init();

	/* Initialize the real time clock */

	clkinit();

	for (i = 0; i < NDEVS; i++) {
		init(i);
	}
	return;
}

int32	stop(char *s)
{
	kprintf("%s\n", s);
	kprintf("looping... press reset\n");
	while(1)
		/* Empty */;
}

int32	delay(int n)
{
	DELAY(n);
	return OK;
}
