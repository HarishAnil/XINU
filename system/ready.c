/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
qid16	userlist;			/* Index of user processes list	*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	//kprintf("\nCall from ready.c: Putting in ready queue for pid:%d\n", pid);	
	long int start = ctr1000;
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	prptr->ready+=1;
	
	if(prptr->is_userproc == 0) {
		//kprintf("\n%d::Call from ready.c: inserting the system process %s into the readylist queue\n",ctr1000,proctab[pid].prname);		
		insert(pid, readylist, prptr->prprio);
	}
	else {
		//kprintf("\n%d::Call from ready.c: inserting the user process: %s into the userlist queue",ctr1000,proctab[pid].prname);
		insert(pid, userlist, prptr->runtime_remaining);
	}
	prptr->ready_time += (ctr1000 - start);	
	//kprintf("\n%d::Call from ready.c: insertion complete...starting to reschedule",ctr1000);
	if(prptr->is_userproc == 1) {
		//kprintf("\n%d::Call from ready.c: user process: %s insertion complete...starting to reschedule",ctr1000,prptr->prname);
		resched();
//	printf("\nuser process: %s reschedule complete",prptr->);
	}
	//kprintf("\nreschedule complete\n");
	else {
		//kprintf("\n%d::call from ready.c: system process: %s insertion complete...starting to reschedule",ctr1000,prptr->prname);		
		resched();
	}
	return OK;
}
