#include <xinu.h>

process	main(void)
{
  //test case 1  trigger
  main_basic();

  //test case 2 trigger
  main_perf();
  
  //test case 3 for deadlocks
  main_deadlock();
  //to let the above testcase complete
  sleepms(1000);

  //test case 3 for trylocks
  main_trylock();
  sleepms(1000);

  //test case 4 for priority inversion
  main_priority_inversion();

  sleepms(10000);

    recvclr();
  resume(create(shell, 8192, 50, "shell", 1, CONSOLE));
  
	// Wait for shell to exit and recreate it 

	while (TRUE){
	receive();
	sleepms(200);
	kprintf("\n\nMain process recreating shell\n\n");
	resume(create(shell, 4096, 10, "shell", 1, CONSOLE));
	}
  
  return OK;
    
}
