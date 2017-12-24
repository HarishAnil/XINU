/*  main.c  - main */

#include <xinu.h>

void print_ready_list()
{
	int	curr;			/* Runs through items in a queue*/	
	
	char *pstate[]	= {		/* names for process states	*/
		"free ", "curr ", "ready", "recv ", "sleep", "susp ",
		"wait ", "rtime"};

	if (isbadqid(readylist)) {
		kprintf("Invalid queue...exiting...\n");
	}	
	else{
		if(queuetab[readylist].qnext == EMPTY)
		{
			kprintf("No processes in the readylist...exiting...\n");
		}		
		else{
			curr = firstid(readylist);
			if((curr >= NPROC) || (curr < 0))
				kprintf("invalid prcoess..exiting...");
			else{
				printf("%10s %10s %10s %10s %10s %10s %10s\n","PRPID","PRSTATE","PRPRIO","PRSTKPTR","PRSTKBASE","PRSTKLEN","PRNAME");
				while (1) {
					if(proctab[curr].prstate == 2 )
						kprintf("%10d %10s %10d %10X %10X %10d %10s\n",curr,pstate[(int)proctab[curr].prstate],proctab[curr].prprio,proctab[curr].prstkptr,proctab[curr].prstkbase,proctab[curr].prstklen,proctab[curr].prname);
					if((int)queuetab[curr].qnext == (int)queuetail(readylist))
						break;
					else
						curr = queuetab[curr].qnext;
				}
			}
		}
	}
	return;
}


void print_user_list()
{
	int	curr;			/* Runs through items in a queue*/	
	
	char *pstate[]	= {		/* names for process states	*/
		"free ", "curr ", "ready", "recv ", "sleep", "susp ",
		"wait ", "rtime"};

	if (isbadqid(userlist)) {
		kprintf("\nInvalid queue...exiting...\n");
	}	
	else{
		if(queuetab[userlist].qnext == EMPTY)
		{
			kprintf("\nNo processes in the userlist...exiting...\n");
		}		
		else{
			curr = firstid(userlist);
			if((curr >= NPROC) || (curr < 0))
				kprintf("\ninvalid prcoess..exiting...");
			else{
				kprintf("\n%10s %10s %10s %10s %10s %10s %10s","PRPID","PRSTATE","RUNTIME","PRSTKPTR","PRSTKBASE","PRSTKLEN","PRNAME");
				while (1) {
					if(proctab[curr].prstate == 2 )
						kprintf("\n%10d %10s %10d %10X %10X %10d %10s",curr,pstate[(int)proctab[curr].prstate],proctab[curr].runtime_remaining,proctab[curr].prstkptr,proctab[curr].prstkbase,proctab[curr].prstklen,proctab[curr].prname);
					if((int)queuetab[curr].qnext == (int)queuetail(userlist))
						break;
					else
						curr = queuetab[curr].qnext;
				}
			}
		}
	}
	return;
}




process	main(void)
{
	
	pid32 pid1,pid2,pid3,pid4;
	int s1_time = 20;
	
	pid1 = create_user_proc(timed_execution,1024,40,"fun_A",1,100);
	pid2 = create_user_proc(timed_execution,1024,35,"fun_B",1,100);
	resume(pid1);
	resume(pid2);
	
	pid3 = create_user_proc(timed_execution,1024,25,"fun_C",1,100);
	pid4 = create_user_proc(timed_execution,1024,30,"fun_D",1,100);

	
	resume(pid3);
	resume(pid4);


	sleepms(s1_time);
	
	return OK;
    
}
