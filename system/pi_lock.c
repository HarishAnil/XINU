#include<xinu.h>

void pi_init(pi_lock_t* l)
{
  l->taken = 0;
  l->wait_queue = newqueue();
  l->owner = 0;    //No owner initially, hence 0
  l->owner_orig_prio = -100;
}

void pi_lock(pi_lock_t* l)
{
  intmask mask;
  while(test_and_set(&l->taken, 1) == 1)
  {
    mask = disable();
    //Add current process to the wait queue if lock unavailable
    enqueue(getpid(), l->wait_queue);
    //Make the process WAITING as it waits for the lock
    proctab[getpid()].prstate = PR_WAIT;

    // Check if the priority of the waiting process is greater than the owner
    if(proctab[getpid()].prprio > proctab[l->owner].prprio)
    {//Raise the owner's priority to the priority of the waiting process
      //Remove the owner from the ready list
      getitem(l->owner);
      //Assign new priority to the owner of the process
      proctab[l->owner].prprio = proctab[getpid()].prprio;
      //Re-insert the owner in the ready list with new priority
      insert(l->owner, readylist, proctab[l->owner].prprio);
    }

    resched();
    restore(mask);
  }
  //If lock is available, set the owner of lock
  l->owner = getpid();
  l->owner_orig_prio = proctab[getpid()].prprio;
  //kprintf("Acquired lock by pid: %d\n", currpid);
}

void pi_unlock(pi_lock_t* l)
{
  //kprintf("Releasing lock by pid %d\n", currpid);
  pid32 pid;
      intmask mask = disable();
  l->taken = 0;

  //Check if the owner has a boosted priority as per priority inheritance protocol
  /*
  if(l->owner == getpid() && proctab[getpid()].prprio != l->owner_orig_prio)
  {
    //Restore the original priority of the owner
    proctab[getpid()].prprio = l->owner_orig_prio;
    l->owner = 0;
    l->owner_orig_prio = -100;
  }
  */
  //Restore the original priority of the owner
  proctab[getpid()].prprio = l->owner_orig_prio;
  l->owner = 0;
  l->owner_orig_prio = -100;
  if(nonempty(l->wait_queue))
  {
    pid = dequeue(l->wait_queue);
    if(isbadpid(pid))
    {
      kprintf("Bad pid: %u\n", pid);
      //return SYSERR;
    }
    ready(pid);
  }
      restore(mask);
}


