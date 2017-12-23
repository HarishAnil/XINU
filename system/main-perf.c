#include <xinu.h>

uint64 sl_done = 0;
uint64 bwf_done = 0;

sl_lock_t lock1;
bwf_lock_t lock2;

uint64 start1 = 0, end1 = 0, start2 = 0, end2 = 0;
uint32 cycles_high1, cycles_low1, cycles_high2, cycles_low2;

void task_sl_lock_perf()
{
  uint32 i;
  
  sl_lock(&lock1);
  //kprintf("Entering critical section - pid: %u\n", getpid());
  for(i = 0; i < 100000; i++)
    ;         //Just spin for 1 million cycles
  //kprintf("Exiting critical section - pid: %u\n", getpid());
  sl_unlock(&lock1);
  sl_done++;
 }

void task_bwf_lock_perf()
{
  uint32 i;

  bwf_lock(&lock2);
  //kprintf("Entering critical section - pid: %u\n", getpid());
  for(i = 0; i < 100000; i++)
    ;         //Just spin for 1 million cycles
  //kprintf("Exiting critical section - pid: %u\n", getpid());
  bwf_unlock(&lock2);
  bwf_done++;
}

void main_perf()
{
  int i;
  for(i=0; i<5; i++){
    resume(create(task_sl_lock_perf, 1024, 15, "Proc", 0, NULL));
  }

  READ_RDTSC_TIMER(cycles_high1, cycles_low1); // start timing
  while(sl_done < 5)
    sleepms(50);

  READ_RDTSC_TIMER(cycles_high2, cycles_low2); // end timing

  start1 = ( ((uint64)cycles_high1 << 32) + cycles_low1 );
  end1 = ( ((uint64)cycles_high2 << 32) + cycles_low2 );

  printf("\n\nClock cycles for spin lock: %u, %u\n", (uint32)((end1 - start1) >> 32), (uint32)(end1 - start1));
  
  for(i=0; i<5; i++){
    resume(create(task_bwf_lock_perf, 1024, 15, "Proc", 0, NULL));
  }  

  READ_RDTSC_TIMER(cycles_high1, cycles_low1); // start timing

  while(bwf_done < 5)
    sleepms(50);

  READ_RDTSC_TIMER(cycles_high2, cycles_low2); // end timing

  start2 = ( ((uint64)cycles_high1 << 32) + cycles_low1 );
  end2 = ( ((uint64)cycles_high2 << 32) + cycles_low2 );
  printf("Clock cycles for busy wait free: %u,%u\n", (uint32)((end2 - start2)>>32), (uint32)(end2 - start2));
  
  //printf("\nClock cycles saved with busy wait free: %d", (end1-start1)-(end2-start2));
}
