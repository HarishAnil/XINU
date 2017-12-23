/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	uint32 start = ctr1000;
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	proctab[currpid].ready+=1;                          /* Since the state changes, increase the counter*/
	insert(pid, readylist, prptr->prprio);
	resched();
	proctab[currpid].ready_time += (ctr1000 - start);
	return OK;
}
