/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
qid16	userlist;			/* Index of user processes list	*/
qid16	userlist2;
qid16 	userlist3;
qid16	userlist4;

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	//kprintf("\nCall from ready.c: Putting in ready queue for pid:%d\n", pid);	
	long int start = ctr1000;
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	prptr->ready+=1;
	
	if(prptr->is_userproc == 0) {
		//kprintf("\n%d::Call from ready.c: inserting the system process %s into the readylist queue\n",ctr1000,proctab[pid].prname);		
		insert(pid, readylist, prptr->prprio);
	}
	else {
		//kprintf("\n%d::Call from ready.c: inserting the user process: %s into the userlist queue",ctr1000,proctab[pid].prname);
		switch(proctab[currpid].qnum){
			case 1: insert(pid,userlist,prptr->runtime_remaining);
				break;
			case 2: insert(pid,userlist2,prptr->runtime_remaining);
				break;
			case 3: insert(pid,userlist3,prptr->runtime_remaining);
				break;
			case 4: insert(pid,userlist4,prptr->runtime_remaining);
				break;
			default:kprintf("\n%d::Inserting into the default queue which is 1..this case should never occur",ctr1000);
		}
	}
	prptr->ready_time += (ctr1000 - start);	
	resched();
	return OK;
}
