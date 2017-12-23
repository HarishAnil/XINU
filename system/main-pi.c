#include <xinu.h>

bwf_lock_t lockb;
bwf_lock_t lockb1;
bwf_lock_t lockb2;

pi_lock_t lockp;
pi_lock_t lockp1;
pi_lock_t lockp2;

void task_bwf_lock(bwf_lock_t *);
void task_pi_lock(pi_lock_t *);
void task_no_lock(void);
void task_bwf_2_locks(bwf_lock_t *, bwf_lock_t *);
void task_pi_2_locks(pi_lock_t *, pi_lock_t *);

// Test case 1 with bwf_lock that results in priority inversion
void task_bwf_lock(bwf_lock_t *lock)
{
  uint32 i;

  bwf_lock(lock);
  kprintf("Enter - pid: %u\n", getpid());
  for(i = 0; i < 1000000; i++)
    ;         //Just spin for 1 million cycles
  kprintf("Exit - pid: %u\n", getpid());
  bwf_unlock(lock);

}

// Test case 2 with pi_lock with priority inheritance
void task_pi_lock(pi_lock_t *lock)
{
  uint32 i;

  pi_lock(lock);
  kprintf("Enter - pid: %u\n", getpid());
  for(i = 0; i < 1000000; i++)
    ;         //Just spin for 1 million cycles
  kprintf("Exit - pid: %u\n", getpid());
  pi_unlock(lock);

}

void task_no_lock()
{
  uint32 i;
  kprintf("Enter - pid: %u\n", getpid());
  for(i = 0; i < 10000; i++)
    ;         //Do noth
  kprintf("Exit - pid: %u\n", getpid());
}

// Test case 3 with bwf_lock and 2 locks
void task_bwf_2_locks(bwf_lock_t *lock1, bwf_lock_t *lock2)
{
  uint32 i;

  bwf_lock(lock1);
  bwf_lock(lock2);
  kprintf("Enter - pid: %u\n", getpid());
  for(i = 0; i < 1000000; i++)
    ;         //Just spin for 1 million cycles
  kprintf("Exit - pid: %u\n", getpid());
  bwf_unlock(lock2);
  bwf_unlock(lock1);

}

// Test case 4 with two pi_locks
void task_pi_2_locks(pi_lock_t *lock1, pi_lock_t *lock2)
{
  uint32 i;

  pi_lock(lock1);
  pi_lock(lock2);
  kprintf("Enter - pid: %u\n", getpid());
  for(i = 0; i < 1000000; i++)
    ;         //Just spin for 1 million cycles
  kprintf("Exit - pid: %u\n", getpid());
  pi_unlock(lock2);
  pi_unlock(lock1);

}

void main_priority_inversion()
{
  bwf_init(&lockb);
  bwf_init(&lockb1);
  bwf_init(&lockb2);
  pi_init(&lockp);
  pi_init(&lockp1);
  pi_init(&lockp2);

  //Test case 1 with priority inversion
  resume(create(task_bwf_lock, 4096, 5, "Process 1", 1, &lockb));
  sleepms(10);
  resume(create(task_bwf_lock, 4096, 15, "Process 2", 1, &lockb));
  resume(create(task_no_lock, 4096, 10, "Process 3", 0));

  sleepms(1000);
  printf("\n\n\n");

  //Test case 2 with priority inheritance
  resume(create(task_pi_lock, 4096, 5, "Process 1", 1, &lockp));
  sleepms(10);
  resume(create(task_pi_lock, 4096, 15, "Process 2", 1, &lockp));
  resume(create(task_no_lock, 4096, 10, "Process 3", 0));

  sleepms(1000);
  printf("\n\n\n");

  //Test case 3 with priority inversion and chain forming
  resume(create(task_bwf_lock, 4096, 5, "Process 1", 1, &lockb1));
  resume(create(task_bwf_lock, 4096, 5, "Process 2", 1, &lockb2));
  sleepms(10);
  resume(create(task_bwf_2_locks, 4096, 10, "Process 3", 2, &lockb1, &lockb2));

  sleepms(1000);
  printf("\n\n\n");

// Test case 4 with priority inheritance and chain forming
  resume(create(task_pi_lock, 4096, 5, "Process 1", 1, &lockp1));
  resume(create(task_pi_lock, 4096, 5, "Process 2", 1, &lockp2));
  sleepms(10);
  resume(create(task_pi_2_locks, 4096, 10, "Process 3", 2, &lockp1, &lockp2));

}


