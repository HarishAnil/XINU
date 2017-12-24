/* resume.c - resume */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  resume  -  Unsuspend a process, making it ready
 *------------------------------------------------------------------------
 */
pri16	resume(
	  pid32		pid		/* ID of process to unsuspend	*/
	)
{
	//kprintf("\nResuming process with pid: %d\n",pid);	
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	prio;			/* Priority to return		*/
	uint32 runtime_remaining; 	/* Run time remaining to return for user processes*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (pri16)SYSERR;
	}
	prptr = &proctab[pid];
	if (prptr->prstate != PR_SUSP) {
		restore(mask);
		return (pri16)SYSERR;
	}

	if(proctab[pid].is_userproc == 1){
		//runtime_remaining = prptr->runtime_remaining;
		//kprintf("\nPutting the user process %s into ready state",proctab[pid].prname);
		ready(pid);
		restore(mask);
		return runtime_remaining;
	}

	else {
		prio = prptr->prprio;		/* Record priority to return	*/
		//kprintf("\nPutting the system process %s into ready state\n",proctab[pid].prname);
		ready(pid);
		restore(mask);
		return prio;
	
	}
}
