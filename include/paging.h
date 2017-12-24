/* paging.h */
typedef uint16 status_t;

/* Structure for a page directory entry */

typedef struct {

  unsigned int pd_pres	: 1;		/* page table present?			*/
  unsigned int pd_write : 1;		/* page is writable?			*/
  unsigned int pd_user	: 1;		/* is use level protection?		*/
  unsigned int pd_pwt	: 1;		/* write through cachine for pt?*/
  unsigned int pd_pcd	: 1;		/* cache disable for this pt?	*/
  unsigned int pd_acc	: 1;		/* page table was accessed?		*/
  unsigned int pd_mbz	: 1;		/* must be zero					*/
  unsigned int pd_fmb	: 1;		/* four MB pages?				*/
  unsigned int pd_global: 1;		/* global (ignored)				*/
  unsigned int pd_avail : 3;		/* for programmer's use, you can use 1 as valid bit		*/
  unsigned int pd_base	: 20;		/* location of page table?		*/
} pd_t;

/* Structure for a page table entry */

typedef struct {

  unsigned int pt_pres	: 1;		/* page is present?				*/
  unsigned int pt_write : 1;		/* page is writable?			*/
  unsigned int pt_user	: 1;		/* is use level protection?		*/
  unsigned int pt_pwt	: 1;		/* write through for this page? */
  unsigned int pt_pcd	: 1;		/* cache disable for this page? */
  unsigned int pt_acc	: 1;		/* page was accessed?			*/
  unsigned int pt_dirty : 1;		/* page was written?			*/
  unsigned int pt_mbz	: 1;		/* must be zero					*/
  unsigned int pt_global: 1;		/* should be zero in 586		*/
  unsigned int pt_avail : 3;		/* for programmer's use, you can use 1 as valid bit	*/
  unsigned int pt_base	: 20;		/* location of page?			*/
} pt_t;

typedef struct{
  unsigned int pg_offset : 12;		/* page offset			*/
  unsigned int pt_offset : 10;		/* page table offset		*/
  unsigned int pd_offset : 10;		/* page directory offset	*/
} virt_addr_t;


//data structure to manage FFS, feel free to change 
typedef struct{
  int fr_status;			/* MAPPED or UNMAPPED		*/
  int fr_pid;				/* process id using this frame  */
  int fr_vpno;				/* corresponding virtual page no*/
  int fr_refcnt;			/* reference count		*/
  int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
  int fr_dirty;
}fr_map_t;

/* Prototypes for required API calls */
// you fill below with prototypes of vcreate vmalloc vfree .... as specified in HW4

char *vmalloc(uint32);
char *vmalloc_shared(pid32, uint32, char*);


/* given calls for dealing with backing store */

#define PAGE_SIZE	4096	/* number of bytes per page	*/
#define MAX_HEAP_SIZE   4096
#define MAX_FSS_SPACE   2048

//swap daemon
#define SD_SLEEP_TIME   10
#define LW		10
#define HW		20


// Added by me
#define FRAME_BEGIN 1024
#define NUM_PAGE_ENTRIES 1024
#define NFRAMES 1024	
#define FFS_SIZE 32
#define FFS_START 2048
#define DISK_SIZE 4096
#define DISK_START 4096
#define VIRT_HEAP_START 8192

#define FR_UNMAPPED	0
#define FR_MAPPED	1

#define PAGE_DIR_TYPE 1
#define PAGE_TBL_TYPE 2
#define PAGE_TYPE 3

#define get_pd_bits(addr) (addr >> 22)
#define get_pt_bits(addr) ((addr << 10) >> 22)
#define get_offset_bits(addr) ((addr << 22) >> 22)
#define get_vpno(addr) addr>>12


extern fr_map_t frame_tab[];
extern fr_map_t ffs_tab[];
extern fr_map_t disk_tab[];

extern uint32 num_free_ffs_frames;

syscall frame_tab_init();
void pfintr();
int pfint();

uint32 get_frame_for_PDPT(int);
uint32 get_frame_for_heap(uint32);
syscall initialize_page_dir(pd_t *);
syscall initialize_page_table(pt_t *, int);
syscall add_directory_entry(pd_t*, uint32, int);
syscall map_page(pt_t*, uint32);

status_t insert_into_llist(struct shared_mem*, struct shared_mem*);

void swap_daemon();
