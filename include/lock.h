/******************** for p3 and p4 **********************/
#define MAX_LOCKS 40

/* Macro function for measuring cycles for system calls */
#define READ_RDTSC_TIMER(cycles_high, cycles_low)  asm volatile ("RDTSC\n\t" "mov %%edx, %0\n\t" "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low));

/* in file testandset.S */
extern uint32 test_and_set(uint32 *, uint32);

/****************** for p2 and p3 *******************/
typedef struct sl_lock_t{
  
  uint32 flag;
  
}sl_lock_t;

extern sl_lock_t lock;

extern void sl_init(sl_lock_t *);
extern void sl_lock(sl_lock_t *);
extern void sl_unlock(sl_lock_t *);

typedef struct bwf_lock_t{

  uint32 taken;
  qid16 wait_queue;

} bwf_lock_t;

extern void bwf_init(bwf_lock_t *);
extern void bwf_lock(bwf_lock_t *);
extern void bwf_unlock(bwf_lock_t *);

/* in main.c*/
extern void main_basic(void);
/* in main-perf.c*/
extern void main_perf(void);

/************** for p4 *******************/
typedef struct al_lock_t{
  
  uint32 flag;
  int index;
  qid16 wait_queue;
  
}al_lock_t;

extern void al_init(al_lock_t *);
extern void al_lock(al_lock_t *);
extern void al_unlock(al_lock_t *);
extern bool8 al_trylock(al_lock_t *);

/* in file main-deadlock.c*/
extern void main_deadlock(void);
extern void main_trylock(void);
/* in file clkhandler.c*/
extern void deadlock_detector(void);

extern uint8 allocation[NPROC][MAX_LOCKS];
extern uint8 request[NPROC][MAX_LOCKS];
extern uint8 available[MAX_LOCKS];
extern uint8 work[MAX_LOCKS];
extern uint8 finish[NPROC];

extern uint8 global_counter;


/*************** for p5 *********************/
typedef struct pi_lock_t{
  uint32 taken;
  qid16 wait_queue;
  pid32 owner;
  pri16 owner_orig_prio;
}pi_lock_t;

extern void pi_init(pi_lock_t *);
extern void pi_lock(pi_lock_t *);
extern void pi_unlock(pi_lock_t *);

extern void main_priority_inversion(void);

