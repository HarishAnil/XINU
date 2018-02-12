/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;
#define ta1 (TIME_ALLOTMENT)
#define ta2 (TIME_ALLOTMENT*2)
#define ta3 (TIME_ALLOTMENT*4)
#define ta4 (TIME_ALLOTMENT*8)
 

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */

void	resched(void)		/* Assumes interrupts are disabled	*/
{
	long int start = ctr1000;
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	struct procent *prptr;
	int temp;

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	/******************************BOOST PROCEDURE****************************************************************/
	if(boost == PRIORITY_BOOST_PERIOD){
		boost = 0;
		if(nonempty(userlist)){
			temp = firstid(userlist);
			while(temp != lastid(userlist)){
				prptr = &proctab[temp];
				prptr->ta = TIME_ALLOTMENT;
				temp = queuetab[temp].qnext;
			}
		}
		while(!isempty(userlist2)){
			temp = dequeue(userlist2);
			enqueue(temp,userlist);
			prptr = &proctab[temp];
			prptr->qnum = 1;
			prptr->ta = TIME_ALLOTMENT;
		}
		while(!isempty(userlist3)){
			temp = dequeue(userlist3);
			enqueue(temp,userlist);
			prptr = &proctab[temp];
			prptr->qnum = 1;
			prptr->ta = TIME_ALLOTMENT;
		}
		while(!isempty(userlist4)){
			temp = dequeue(userlist4);
			enqueue(temp,userlist);
			prptr = &proctab[temp];
			prptr->qnum = 1;
			prptr->ta = TIME_ALLOTMENT;
		}
	
	}
	/******************************END OF BOOST PROCEDURE********************************************************/
	
	/*****************************USER PROCESS********************************************************************/
	else if(ptold->is_userproc == 1) {						//check if user process calls reschedule

		if((proctab[currpid].ta == 0) && (proctab[currpid].prstate = PR_CURR) ){
			//kprintf("\nTIME ALLOTMENT EXPIRED...TIME TO EXPIRE");
			switch(proctab[currpid].qnum){
				case 1: insert(currpid,userlist2,ptold->runtime_remaining);
					proctab[currpid].qnum = 2;
					proctab[currpid].ta = ta2;
					if(proctab[currpid].burst_duration == 0)
						proctab[currpid].burst_done = 1;
					break;
				case 2: insert(currpid,userlist3,ptold->runtime_remaining);
					proctab[currpid].qnum = 3;
					proctab[currpid].ta = ta3;
					if(proctab[currpid].burst_duration == 0)
						proctab[currpid].burst_done = 1;
					break;
				case 3: insert(currpid,userlist4,ptold->runtime_remaining);
					proctab[currpid].qnum = 4;
					proctab[currpid].ta = ta4;
					if(proctab[currpid].burst_duration == 0)
						proctab[currpid].burst_done = 1;
					break;
				case 4: proctab[currpid].ta = ta4;
					if(proctab[currpid].burst_duration == 0)
						proctab[currpid].burst_done = 1;
					break;
				default://kprintf("\n%d::Call from resched.c: TA expired: Inserting into the default queue which is 1..this case should never occur",ctr1000);
					break;
			}
			if( (nonempty(readylist)) && (firstid(readylist)!=NULLPROC) ){
				currpid = dequeue(readylist);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				kprintf("-%d",ctr1000);
				//kprintf("\n%d::Context switching from %s to %s",ctr1000,ptold->prname,ptnew->prname);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				return;				
			}
			else if(nonempty(userlist)){
				currpid = dequeue(userlist);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				//kprintf("\n%d::Context switching from %s in queue %d to %s in queue %d",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
				kprintf("-%d",ctr1000);
				kprintf("\nP%d:running::%d",currpid,ctr1000);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				return;				
			}
			else if(nonempty(userlist2)){
				currpid = dequeue(userlist2);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				//kprintf("\n%d::Context switching from %s in queue %d to %s in queue %d",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
				kprintf("-%d",ctr1000);
				kprintf("\nP%d:running::%d",currpid,ctr1000);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				return;
			}
			else if(nonempty(userlist3)){
				currpid = dequeue(userlist3);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				//kprintf("\n%d::Context switching from %s in queue %d to %s in queue %d",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
				kprintf("-%d",ctr1000);
				kprintf("\nP%d:running::%d",currpid,ctr1000);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				return;
			}
			else if(nonempty(userlist4)){
				currpid = dequeue(userlist4);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				//kprintf("\n%d::Context switching from %s in queue %d to %s in queue %d",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
				kprintf("-%d",ctr1000);
				kprintf("\nP%d:running::%d",currpid,ctr1000);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				return;
			}
			else{
				currpid = dequeue(readylist);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				//kprintf("\n%d::Context switching from %s in queue %d to %s in queue %d",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
				kprintf("-%d",ctr1000);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				return;
			}
		}
		else if(ptold->prstate == PR_CURR){
			if(firstid(readylist) != NULLPROC) {	//check for system processes in readylist except null
				ptold->prstate = PR_READY;
				if(ptold->qnum == 1)
					insert(currpid, userlist,ptold->runtime_remaining);
				else if(ptold->qnum == 2)
					insert(currpid, userlist2,ptold->runtime_remaining);
				else if(ptold->qnum == 3)
					insert(currpid, userlist3,ptold->runtime_remaining);
				else if(ptold->qnum == 4)
					insert(currpid, userlist4,ptold->runtime_remaining);

				currpid = dequeue(readylist);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				//kprintf("\n%d::Context switching from %s in queue %d to %s in queue %d",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
				kprintf("-%d",ctr1000);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);	
			}
			else{
				if(nonempty(userlist)){
					switch(proctab[currpid].qnum){
						case 1:	insert(currpid, userlist,ptold->runtime_remaining);break;
						case 2:	insert(currpid, userlist2,ptold->runtime_remaining);break;
						case 3:	insert(currpid, userlist3,ptold->runtime_remaining);break;
						case 4: insert(currpid, userlist4,ptold->runtime_remaining);break;
					}
					currpid = dequeue(userlist);
					ptold->prstate = PR_READY;				//else, ctx sw to new process in readylist
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					kprintf("-%d",ctr1000);
					kprintf("\nP%d:running::%d",currpid,ctr1000);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);							
				}
				else if(nonempty(userlist2)){
					switch(proctab[currpid].qnum){
						case 1:	return;
						case 2:	insert(currpid, userlist2,ptold->runtime_remaining);break;
						case 3:	insert(currpid, userlist3,ptold->runtime_remaining);break;
						case 4: insert(currpid, userlist4,ptold->runtime_remaining);break;
					}
					currpid = dequeue(userlist2);
					ptold->prstate = PR_READY;				//else, ctx sw to new process in readylist
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					kprintf("-%d",ctr1000);
					kprintf("\nP%d:running::%d",currpid,ctr1000);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);							
									
				}
				else if(nonempty(userlist3)){
					switch(proctab[currpid].qnum){
						case 1:	return;
						case 2:	return;
						case 3:	insert(currpid, userlist3,ptold->runtime_remaining);break;
						case 4: insert(currpid, userlist4,ptold->runtime_remaining);break;
					}
					currpid = dequeue(userlist3);
					ptold->prstate = PR_READY;				//else, ctx sw to new process in readylist
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					kprintf("-%d",ctr1000);
					kprintf("\nP%d:running::%d",currpid,ctr1000);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);							
								
				}
				else if(nonempty(userlist4)){
					switch(proctab[currpid].qnum){
						case 1:	return;
						case 2:	return;
						case 3:	return;
						case 4: insert(currpid, userlist4,ptold->runtime_remaining);break;
					}
					currpid = dequeue(userlist4);
					ptold->prstate = PR_READY;				//else, ctx sw to new process in readylist
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					kprintf("-%d",ctr1000);
					kprintf("\nP%d:running::%d",currpid,ctr1000);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);							
								
				}
				else
					return;
			}
		}
		else{						//user proc not current
			if(firstid(readylist) != NULLPROC) {	//check for system processes in readylist except null
				currpid = dequeue(readylist);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				ptnew->resched+=1;
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ptold->resched_time += (ctr1000 - start);
				//kprintf("\n%d::Context switching from %s in queue %d to %s in queue %d",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);	
			}
			else {
				if((isempty(userlist)) && (isempty(userlist2)) && (isempty(userlist3)) && (isempty(userlist4))){	//case of null ctx sw
					//print_user_list();
					currpid = dequeue(readylist);		//dequeuing null
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					//kprintf("\n%d::Context switching from %s in queue %d to %s",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				}
				else{
					if(nonempty(userlist))
						currpid = dequeue(userlist);
					else if(nonempty(userlist2))
						currpid = dequeue(userlist2);
					else if(nonempty(userlist3))
						currpid = dequeue(userlist3);
					else
						currpid = dequeue(userlist4);
						
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					//kprintf("\n%d::Context switching from %s in queue %d to %s in queue %d",ctr1000,ptold->prname,ptold->qnum,ptnew->prname,ptnew->qnum);
					kprintf("-%d",ctr1000);
					kprintf("\nP%d:running::%d",currpid,ctr1000);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				}
			}
		}

	}
	/*********************************END OF USER PROCESS SCHEDULING***************************************************/
	
	/*****************************SYSTEM PROCESS********************************************************************/
	else {
		if(currpid == NULLPROC){
			if(ptold->prstate == PR_CURR){
				if(isempty(readylist)){
					if( (nonempty(userlist)) || (nonempty(userlist2)) || (nonempty(userlist3)) || (nonempty(userlist4)) ){
						if(nonempty(userlist))
							currpid = dequeue(userlist);
						else if(nonempty(userlist2))
							currpid = dequeue(userlist2);
						else if(nonempty(userlist3))
							currpid = dequeue(userlist3);
						else
							currpid = dequeue(userlist4);

						ptnew = &proctab[currpid];
						ptnew->prstate = PR_CURR;
						ptnew->resched+=1;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						ptold->resched_time += (ctr1000 - start);
						kprintf("\nP%d:running::%d",currpid,ctr1000);
						//kprintf("\n%d::Context switching from %s to %s",ctr1000,ptold->prname,ptnew->prname);
						ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
					}
					else{
						//kprintf("\n%d::null running",ctr1000);
						return;
					}
				}
				else {
					insert(currpid,readylist,ptold->prprio);
					currpid = dequeue(readylist);
					ptnew = &proctab[currpid];
					ptnew->prstate = PR_CURR;
					ptnew->resched+=1;
					preempt = QUANTUM;		/* Reset time slice for process	*/
					ptold->resched_time += (ctr1000 - start);
					//kprintf("\n%d::Context switching from %s to %s",ctr1000,ptold->prname,ptnew->prname);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);				
				}
			}
		}
		else{							//not null
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
					//kprintf("\n%d::Context switching from %s to %s",ctr1000,ptold->prname,ptnew->prname);
					ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				}
			}
			else{						//not null not current
				if(firstid(readylist) == NULLPROC){
					if( (nonempty(userlist)) || (nonempty(userlist2)) || (nonempty(userlist3)) || (nonempty(userlist4)) ){
						if(nonempty(userlist))
							currpid = dequeue(userlist);
						else if(nonempty(userlist2))
							currpid = dequeue(userlist2);
						else if(nonempty(userlist3))
							currpid = dequeue(userlist3);
						else
							currpid = dequeue(userlist4);
							
						//kprintf("\ncurrpid obtained is %d with name %s",currpid,proctab[currpid].prname);
						ptnew = &proctab[currpid];
						ptnew->prstate = PR_CURR;
						ptnew->resched+=1;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						ptold->resched_time += (ctr1000 - start);
						kprintf("\nP%d-running::%d",currpid,ctr1000);
						//kprintf("\n%d::Context switching from %s to %s",ctr1000,ptold->prname,ptnew->prname);
						ctxsw(&ptold->prstkptr, &ptnew->prstkptr);				
					}
					else{
						//kprintf("\nOver here:");
						currpid = dequeue(readylist);
						//kprintf("\nDequeud process is: %s",proctab[currpid].prname);
						ptnew = &proctab[currpid];
						ptnew->prstate = PR_CURR;
						ptnew->resched+=1;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						ptold->resched_time += (ctr1000 - start);
						//kprintf("\n%d::Context switching from %s to %s",ctr1000,ptold->prname,ptnew->prname);
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
						//kprintf("\n%d::Context switching from %s to %s",ctr1000,ptold->prname,ptnew->prname);
						ctxsw(&ptold->prstkptr, &ptnew->prstkptr);				
					
				}
			}
		}
	}
	/*****************************************************END OF SYSTEM PROCESS SCHEDULING*********************************************/
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

