/* suspend.c - suspend */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  suspend  -  Suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
syscall	suspend(
	  pid32		pid		/* ID of process to suspend	*/
	)
{
	uint32 start = ctr1000;
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)) {
		restore(mask);
		return SYSERR;
	}

	/* Only suspend a process that is current or ready */
	prptr = &proctab[pid];
	if ((prptr->prstate != PR_CURR) && (prptr->prstate != PR_READY)) {
		restore(mask);
		return SYSERR;
	}
	if (prptr->prstate == PR_READY) {
		getitem(pid);		     /* Remove a ready process	*/
					     /*   from the ready list	*/
		prptr->prstate = PR_SUSP;
		proctab[currpid].suspend+=1; /*Since the state changes, increase the counter*/
	} else {
		prptr->prstate = PR_SUSP;    /* Mark the current process	*/
		proctab[currpid].suspend+=1; /*Since the state changes, increase the counter*/
		resched();		     /*   suspended and resched.	*/
	}
	prio = prptr->prprio;
	restore(mask);
	proctab[currpid].suspend_time += (ctr1000 - start);
	return prio;
}
