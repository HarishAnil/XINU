/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler()
{
	ctr1000++;
	if((boost++) == PRIORITY_BOOST_PERIOD){
		resched();
	}
	
	if((proctab[currpid].is_userproc == 1) && (proctab[currpid].burst_duration > 0)){
		proctab[currpid].burst_duration --;
		proctab[currpid].ta --;
		//kprintf("\n%d::Burst time for current process %s is now %d and alloted time left is: %d and in queue is %d",ctr1000,proctab[currpid].prname,proctab[currpid].burst_duration,proctab[currpid].ta, proctab[currpid].qnum);
		if(proctab[currpid].ta == 0){
			resched();
			//kprintf("\n%d::%s process resumed here",ctr1000,proctab[currpid].prname);
		}
		else if(proctab[currpid].burst_duration == 0){
			proctab[currpid].burst_done = 1;
		}

	}


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
			//kprintf("\n%d::Calling wakeup",ctr1000);			
			wakeup();
		}
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */

	if((--preempt) <= 0) {
		preempt = QUANTUM;
		//if(proctab[currpid].is_userproc == 1)
			//kprintf("\n%d::call from clockhandler.c: TIME SLICE EXPIRED for user process %s..proceed to reschedule\n",ctr1000,proctab[currpid].prname);
		resched();
	}
}
