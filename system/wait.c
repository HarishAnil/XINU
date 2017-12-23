/* wait.c - wait */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wait  -  Cause current process to wait on a semaphore
 *------------------------------------------------------------------------
 */
syscall	wait(
	  sid32		sem		/* Semaphore on which to wait  */
	)
{
        uint32 start = ctr1000;
  	intmask mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	struct	sentry *semptr;		/* Ptr to sempahore table entry	*/

	mask = disable();
	if (isbadsem(sem)) {
		restore(mask);
		return SYSERR;
	}

	semptr = &semtab[sem];
	if (semptr->sstate == S_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--(semptr->scount) < 0) {		/* If caller must block	*/
		prptr = &proctab[currpid];
		prptr->prstate = PR_WAIT;	/* Set state to waiting	*/
		prptr->wait+=1;                 /* Since the state changes, increase the counter*/
		prptr->prsem = sem;		/* Record semaphore ID	*/
		enqueue(currpid,semptr->squeue);/* Enqueue on semaphore	*/
		resched();			/*   and reschedule	*/
	}

	restore(mask);
	prptr->wait_time += ctr1000-start;
	return OK;
}
