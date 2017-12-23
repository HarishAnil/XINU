/* sleep.c - sleep sleepms */

#include <xinu.h>

#define	MAXSECONDS	2147483		/* Max seconds per 32-bit msec	*/

/*------------------------------------------------------------------------
 *  sleep  -  Delay the calling process n seconds
 *------------------------------------------------------------------------
 */
syscall	sleep(
	  int32	delay		/* Time to delay in seconds	*/
	)
{
	uint32 start = ctr1000;
	if ( (delay < 0) || (delay > MAXSECONDS) ) {
		return SYSERR;
	}
	sleepms(1000*delay);
	proctab[currpid].sleep_time += (ctr1000 - start);
	return OK;
}

/*------------------------------------------------------------------------
 *  sleepms  -  Delay the calling process n milliseconds
 *------------------------------------------------------------------------
 */
syscall	sleepms(
	  int32	delay			/* Time to delay in msec.	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32 start = ctr1000;
	if (delay < 0) {
		return SYSERR;
	}

	if (delay == 0) {
		yield();
		return OK;
	}

	/* Delay calling process */

	mask = disable();
	if (insertd(currpid, sleepq, delay) == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	proctab[currpid].prstate = PR_SLEEP;
	proctab[currpid].sleep+=1;
	resched();
	restore(mask);
	proctab[currpid].sleep_time += (ctr1000 - start);
	
	return OK;
}
