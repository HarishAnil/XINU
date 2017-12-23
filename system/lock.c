#include<xinu.h>

void bwf_init(bwf_lock_t* l)
{
	l->taken = 0;
	l->wait_queue = newqueue();
}

void bwf_lock(bwf_lock_t* l)
{
	intmask mask;
	while(test_and_set(&l->taken, 1) == 1)
	{
	  //Add process to the queue if lock unavailable
	  enqueue(getpid(), l->wait_queue);
	  //Make the process WAITING as it waits for the lock
	  proctab[currpid].prstate = PR_WAIT;
	  // kprintf("Putting %s process in Waiting\n", proctab[currpid].prname);
	  mask = disable();
	  resched();
	  restore(mask);
	}
	//kprintf("Acquired lock by pid: %d\n", currpid);
}

void bwf_unlock(bwf_lock_t* l)
{
	//kprintf("Releasing lock by pid %d\n", currpid);
	pid32 pid;
	l->taken = 0;
	if(nonempty(l->wait_queue))
	{
	  pid = dequeue(l->wait_queue);
	  if(isbadpid(pid))
	    kprintf("Bad pid\n");
	  
	  ready(pid);
	  /*
	    proctab[pid].prstate = PR_READY;
	    insert(pid, readylist, proctab[pid].prprio);
	    resched();
	  */
	}
}
