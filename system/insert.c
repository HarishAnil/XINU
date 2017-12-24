/* insert.c - insert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insert  -  Insert a process into a queue in descending key order
 *------------------------------------------------------------------------
 */
status	insert(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Key for the inserted process	*/
	)
{
	int16	curr;			/* Runs through items in a queue*/
	int16	prev;			/* Holds previous node index	*/

	//kprintf("\n%d::Call from insert.c: Caller: %s...inserting process %s into q: %d",ctr1000,proctab[currpid].prname,proctab[pid].prname,q);
	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	curr = firstid(q);
	//if(nonempty(q))
		//kprintf("\n%d::Call from insert.c: first process in q:%d is %s",ctr1000,q,proctab[curr].prname);
	//else
		//kprintf("\n%d::Call from insert.c: Empty queue: %d",ctr1000,q);
	//kprintf("\n%d::Call from insert.c: About to enter while loop!..brace yourself!",ctr1000);
	while (queuetab[curr].qkey >= key) {
		curr = queuetab[curr].qnext;
	}
	//kprintf("\n%d::Call from insert.c: not stuck in while:!",ctr1000);

	/* Insert process between curr node and previous node */

	prev = queuetab[curr].qprev;	/* Get index of previous node	*/
	queuetab[pid].qnext = curr;
	queuetab[pid].qprev = prev;
	queuetab[pid].qkey = key;
	queuetab[prev].qnext = pid;
	queuetab[curr].qprev = pid;
	//kprintf("\n%d::Call from insert.c: About to exit the function insert...insertion of process %s complete",ctr1000,proctab[pid].prname);
	return OK;
}
