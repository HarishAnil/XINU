#include <xinu.h>

void pr_status_syscall_summary() {
	printf("\nOutcome of pr_status_syscall_summary");
	printf("\n%s\t\t%s\t\t%s\t\t%s","pid","syscall","count","average time (ms)");
	printf("\n%s\t\t%s\t\t%s\t\t%s","---", "------","-----", "--------------");
	int i;
	for(i=0; i<NPROC; i++) {
		if((proctab[i].wait > 0)||(proctab[i].kill > 0)||(proctab[i].ready > 0)||(proctab[i].receive > 0)||(proctab[i].suspend > 0)||(proctab[i].recvtime > 0)||(proctab[i].create > 0)||(proctab[i].sleep > 0)||(proctab[i].resched > 0)) {		
			if(proctab[i].wait > 0)
				printf("\n%d\t\t%s\t\t%d\t\t%d",i,"wait",proctab[i].wait,proctab[i].wait_time/proctab[i].wait);
			if(proctab[i].kill > 0)
				printf("\n%d\t\t%s\t\t%d\t\t%d",i,"kill",proctab[i].kill,proctab[i].kill_time/proctab[i].kill);
			if(proctab[i].ready > 0)
				printf("\n%d\t\t%s\t\t%d\t\t%d",i,"ready",proctab[i].ready,proctab[i].ready_time/proctab[i].ready);
			if(proctab[i].receive > 0)
				printf("\n%d\t\t%s\t\t%d\t\t%d",i,"receive",proctab[i].receive,proctab[i].receive_time/proctab[i].receive);
			if(proctab[i].suspend > 0)
				printf("\n%d\t\t%s\t\t%d\t\t%d",i,"suspend",proctab[i].suspend,proctab[i].suspend_time/proctab[i].suspend);
			if(proctab[i].recvtime > 0)
				printf("\n%d\t\t%s\t%d\t\t%d",i,"recvtime",proctab[i].recvtime,proctab[i].recvtime_time/proctab[i].recvtime);
			if(proctab[i].create > 0)
				printf("\n%d\t\t%s\t\t%d\t\t%d",i,"create",proctab[i].create,proctab[i].create_time/proctab[i].create);
			if(proctab[i].sleep > 0)
				printf("\n%d\t\t%s\t\t%d\t\t%d",i,"sleep",proctab[i].sleep,proctab[i].sleep_time/proctab[i].sleep);
			if(proctab[i].resched > 0)
				printf("\n%d\t\t%s\t\t%d\t\t%d",i,"resched",proctab[i].resched,proctab[i].resched_time/proctab[i].resched);
			printf("\n----------------------------------------------------------");
		}
	}
}
