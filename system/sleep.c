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
	long int start = ctr1000;
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

//	if(proctab[currpid].is_userproc == 1)	
//		kprintf("\n%d::Call from sleep.c: Process %s called sleep.c with sleep time:%d", ctr1000,proctab[currpid].prname,delay);

	if (delay < 0) {
		return SYSERR;
	}

	if (delay == 0) {
		yield();
		return OK;
	}

	/* Delay calling process */
	//	kprintf("\n%d::before",ctr1000);
	mask = disable();
	if (insertd(currpid, sleepq, delay) == SYSERR) {
		restore(mask);
		return SYSERR;
	}
	//	kprintf("\n%d::after",ctr1000);

	proctab[currpid].prstate = PR_SLEEP;
	proctab[currpid].sleep+=1;
//	kprintf("\n%d::About to call reschedule from sleep!",ctr1000);
	resched();
	restore(mask);
	return OK;
}
