/* wakeup.c - wakeup */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wakeup  -  Called by clock interrupt handler to awaken processes
 *------------------------------------------------------------------------
 */
void	wakeup(void)
{
	/* Awaken all processes that have no more time to sleep */
	//kprintf("\n%d::Awakenend %s process from sleepq by process %s !!",ctr1000,proctab[firstid(sleepq)].prname,proctab[currpid].prname);

	resched_cntl(DEFER_START);
	while (nonempty(sleepq) && (firstkey(sleepq) <= 0)) {
		//kprintf("\n%d::Awakenend from sleep..process is: %s",ctr1000,proctab[firstid(sleepq)].prname);
//		print_ready_list();
		ready(dequeue(sleepq));
	}

	resched_cntl(DEFER_STOP);
	return;
}
