/* resched.c - resched, resched_cntl */

#include <xinu.h>
#include <string.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */

void	resched(void)		/* Assumes interrupts are disabled	*/
{
	
	long int start = ctr1000;
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		//kprintf("\nRescheduling deferred...recorded the attempt\n");
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	if(ptold->is_userproc == 1) {						//check if user process calls reschedule
		if(ptold->prstate == PR_CURR){
			if(!(strncmp(proctab[firstid(readylist)].prname, "prnull",6)==0)) {	//check for system processes in readylist except null	
				
				ptold->prstate = PR_READY;				//else, ctx sw to new process in readylist
				insert(currpid, userlist, ptold->runtime_remaining); /////////////////////////////THIS IS NEEDED//////////////////////////
				
				currpid = dequeue(readylist);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				kprintf("-%d",ctr1000);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);	
			}
			else{
				
				if(nonempty(userlist)){
					if((ptold->runtime_remaining >= proctab[lastid(userlist)].runtime_remaining) && (ptold->finished_execution == 0)) {
						ptold->prstate = PR_READY;				//else, ctx sw to new process in readylist
						int temp = currpid;
						insert(currpid, userlist, ptold->runtime_remaining);
						currpid = dequeue(userlist);
						ptnew = &proctab[currpid];
						ptnew->prstate = PR_CURR;
						ptnew->resched+=1;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						ptold->resched_time += (ctr1000 - start);
						//kprintf("-%d",ctr1000);
						//kprintf("\nP%d-running::%d",temp,ctr1000);
						ctxsw(&ptold->prstkptr, &ptnew->prstkptr);							
					}
					else
						return;
				}
				else
					return;
			}
		}
		else{
			if(!(strncmp(proctab[firstid(readylist)].prname, "prnull",6)==0)) {	//check for system processes in readylist except null	
				currpid = dequeue(readylist);
				
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				kprintf("-%d",ctr1000);					////////////////////////////////////////////////////////////////
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);	
			}
			else {
				if(isempty(userlist)){
					currpid = dequeue(readylist);				///dequeing null, so time to put it back into readylist
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					//kprintf("-%d",ctr1000);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				}
				else{
					//int temp2 = currpid;					
					currpid = dequeue(userlist);
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					//kprintf("-%d",ctr1000);
					kprintf("\nP%d-running::%d",currpid,ctr1000); ////////////////////////////////////////////////
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				}
			}
		}

	}
	else {
		if(currpid == NULLPROC){
			if(ptold->prstate == PR_CURR){
				if(isempty(readylist)){
					if(nonempty(userlist)){
						//int temp3 = currpid;
						currpid = dequeue(userlist);
						ptnew = &proctab[currpid];
						ptnew->prstate = PR_CURR;
						ptnew->resched+=1;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						ptold->resched_time += (ctr1000 - start);
						//kprintf("\nP%d-running::%d",currpid,ctr1000);
						ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
					}
					else
						return;
					}
				else {
					insert(currpid,readylist,ptold->prprio);
					currpid = dequeue(readylist);
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);				
				}
			}
		}
		else{
			if(ptold->prstate == PR_CURR){
				if(ptold->prprio > firstkey(readylist)){
					return;
				}
				else{
					insert(currpid,readylist,ptold->prprio);
					currpid = dequeue(readylist);
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				}
			}
			else{
				if(firstkey(readylist) == NULLPROC){
					if(nonempty(userlist)){
						//int temp4 = currpid;
						currpid = dequeue(userlist);
						ptnew = &proctab[currpid];
						ptnew->prstate = PR_CURR;
						ptnew->resched+=1;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						ptold->resched_time += (ctr1000 - start);
						kprintf("\nP%d-running::%d",currpid,ctr1000); ////////////////////////////////////////////////////////
						ctxsw(&ptold->prstkptr, &ptnew->prstkptr);				
					}
					else{
						currpid = dequeue(readylist);
						ptnew = &proctab[currpid];
						ptnew->prstate = PR_CURR;
						ptnew->resched+=1;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						ptold->resched_time += (ctr1000 - start);
						ctxsw(&ptold->prstkptr, &ptnew->prstkptr);				
					}
				}
				else{
						currpid = dequeue(readylist);
						ptnew = &proctab[currpid];
						ptnew->prstate = PR_CURR;
						ptnew->resched+=1;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						ptold->resched_time += (ctr1000 - start);
						ctxsw(&ptold->prstkptr, &ptnew->prstkptr);				
					
				}
			}
		}
	}
	
	/* Old process returns here when resumed */
	return;
}
	

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}

