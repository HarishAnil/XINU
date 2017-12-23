#include <xinu.h>

void pr_status_syscall_summary() {
  printf("\n%10s\t%10s\t%10s\t%10s","pid","syscall","count","average time(ms)");
  printf("\n%10s\t%10s\t%10s\t%10s","-----", "-----","-----", "-----");
  uint8 i;
  for(i=0; i<NPROC; i++) {
    if( (proctab[i].wait > 0) || (proctab[i].kill > 0) || (proctab[i].ready > 0) || (proctab[i].receive > 0) || (proctab[i].suspend > 0) || (proctab[i].recvtime > 0) || (proctab[i].create > 0) || (proctab[i].sleep > 0) || (proctab[i].resched > 0) ){		
      if(proctab[i].wait > 0)
	printf("\n%10d\t%10s\t%10d\t%10u",i,"wait",proctab[i].wait,proctab[i].wait_time/proctab[i].wait);
      if(proctab[i].kill > 0)
	printf("\n%10d\t%10s\t%10d\t%10d",i,"kill",proctab[i].kill,proctab[i].kill_time/proctab[i].kill);
      if(proctab[i].ready > 0)
	printf("\n%10d\t%10s\t%10d\t%10d",i,"ready",proctab[i].ready,proctab[i].ready_time/proctab[i].ready);
      if(proctab[i].receive > 0)
	printf("\n%10d\t%10s\t%10d\t%10d",i,"receive",proctab[i].receive,proctab[i].receive_time/proctab[i].receive);
      if(proctab[i].suspend > 0)
	printf("\n%10d\t%10s\t%10d\t%10d",i,"suspend",proctab[i].suspend,proctab[i].suspend_time/proctab[i].suspend);
      if(proctab[i].recvtime > 0)
	printf("\n%10d\t%10s\t%10d\t%10d",i,"recvtime",proctab[i].recvtime,proctab[i].recvtime_time/proctab[i].recvtime);
      if(proctab[i].create > 0)
	printf("\n%10d\t%10s\t%10d\t%10d",i,"create",proctab[i].create,proctab[i].create_time/proctab[i].create);
      if(proctab[i].sleep > 0)
	printf("\n%10d\t%10s\t%10d\t%10d",i,"sleep",proctab[i].sleep,proctab[i].sleep_time/proctab[i].sleep);
      if(proctab[i].resched > 0)
	printf("\n%10d\t%10s\t%10d\t%10d",i,"resched",proctab[i].resched,proctab[i].resched_time/proctab[i].resched);
      printf("\n----------------------------------------------------------");
    }
  }
}
