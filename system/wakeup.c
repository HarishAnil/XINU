/* wakeup.c - wakeup */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wakeup  -  Called by clock interrupt handler to awaken processes
 *------------------------------------------------------------------------
 */
void	wakeup(void)
{
	/* Awaken all processes that have no more time to sleep */
	//kprintf("\n%d::Awakenend %s sleep by process %s !!",ctr1000,firstid(sleepq),currpid);

	resched_cntl(DEFER_START);
	while (nonempty(sleepq) && (firstkey(sleepq) <= 0)) {
		//kprintf("\n%d::Awakenend from sleep!!..process is: %d",ctr1000,firstid(sleepq));
//		print_ready_list();
		ready(dequeue(sleepq));
	}

	resched_cntl(DEFER_STOP);
	return;
}
