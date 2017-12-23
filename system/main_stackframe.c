#include <xinu.h>

int function_two_para(int a, int b){

  unsigned long sum = 0;
  unsigned long i = 0;
  unsigned long *sp;

  asm("movl %%esp, %0\n" :"=r"(sp));  
  sum = a + b;

  printf("\n\nCASE1:AFTER");
  printf("\nStack pointer: %X",sp);
  printf("\nStack contents:");
  printf("\naddress\tvalue");
  printf("\n-------\t-------");
  
  while(i<12)
    {
      if(*sp == STACKMAGIC)
	break;		
      printf("\n%X (%X):%d", sp, *sp,*sp);
      sp++;
      i++;
    }
  
  printf("\nThe sum of the two parameters %d and %d is: %d\n", a, b, sum);
  return OK;
}

int function_three_para(int a, int b, int c){
  unsigned long i;
  unsigned long sum;
  unsigned long *sp;
  asm("movl %%esp, %0\n" :"=r"(sp));

  sum = a + b + c;
  
  printf("\n\nCASE2:AFTER");
  printf("\nStack pointer: %X",sp);
  printf("\nStack contents:");
  printf("\naddress\tvalue");
  printf("\n-------\t-------");
  for (i=0; i<12; i++){
    if(*sp == STACKMAGIC)
      break;		
    printf("\n%X (%X):%d", sp, *sp,*sp);
    sp++;
  }
  printf("\nThe sum of the three parameters %d, %d and %d is: %d\n", a, b, c, sum);
  return OK;
}

process	main(void)
{
  int i;
  unsigned long *sp;

  asm("movl %%esp, %0\n" :"=r"(sp));
    
  printf("\nCASE1:BEFORE");
  printf("\nStack pointer: %X",sp);
  printf("\nStack contents:");
  printf("\naddress\tvalue");
  printf("\n-------\t-------");

  for (i=0; i<=12; i++){
    if(*sp == STACKMAGIC)
      break;		
    printf("\n%X (%X):%d", sp, *sp, *sp);
    sp++;
  }
 
  function_two_para(1, 2);
  
  printf("\nCASE2:BEFORE");

  asm("movl %%esp, %0\n" :"=r"(sp));

  printf("\nStack pointer: %X",sp);
  printf("\nStack contents:");
  printf("\naddress\tvalue");
  printf("\n-------\t-------");
  for (i=0; i<12; i++){
    if(*sp == STACKMAGIC)
      break;		
    printf("\n%X (%X):%d", sp, *sp,*sp);
    sp++;
  }
  
  function_three_para(1, 2, 3);
  
  pr_status_syscall_summary();

  /* Run the Xinu shell */
  recvclr();
  resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

  /* Wait for shell to exit and recreate it */

  while (TRUE) {
    receive();
    sleepms(200);
    kprintf("\n\nMain process recreating shell\n\n");
    resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
  }
	
  return OK;
    
}
