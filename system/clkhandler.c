/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler()
{
	//a = ctr1000;	
	ctr1000++;
	if((proctab[currpid].runtime_remaining > 0) && (proctab[currpid].is_userproc == 1)){
		//kprintf("\n%d::User process: %s has runtime remaining: %d",ctr1000,proctab[currpid].prname,proctab[currpid].runtime_remaining);
		proctab[currpid].runtime_remaining -=1;
	}
	if((proctab[currpid].runtime_remaining == 0) && (proctab[currpid].is_userproc == 1))
	{
		proctab[currpid].finished_execution = 1;
		//kprintf("\nP%d-termination:%d",currpid,ctr1000);
	}
	//b = ctr1000;
	//kprintf("\n1 ms has passed..current time:\n",ctr1000);	
	static	uint32	count1000 = 1000;	/* Count to 1000 ms	*/

	/* Decrement the ms counter, and see if a second has passed */

	if((--count1000) <= 0) {

		/* One second has passed, so increment seconds count */

		clktime++;

		/* Reset the local ms counter for the next second */

		count1000 = 1000;
		//kprintf("\nCurrent process is %s",proctab[currpid].prname);
		//kprintf("\nOne second has passed dear..what are you upto..");
	}

	/* Handle sleeping processes if any exist */

	if(!isempty(sleepq)) {

		/* Decrement the delay for the first process on the	*/
		/*   sleep queue, and awaken if the count reaches zero	*/

		if((--queuetab[firstid(sleepq)].qkey) <= 0) {
			wakeup();
		}
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */

	if((--preempt) <= 0) {
		preempt = QUANTUM;
		//kprintf("\n%d::call from clockhandler.c: TIME SLICE EXPIRED..proceed to reschedule\n",ctr1000);
		resched();
	}
}
