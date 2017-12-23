/* testandset.S - test and set (for x86) */

		.text
		.globl	test_and_set

/*------------------------------------------------------------------------
 * testandset -  X86 test and set implementation; the call is test_and_set(uint32 *ptr, uint32 new_value)
 *------------------------------------------------------------------------
 */
test_and_set:

	pushl	%ebx
	movl	8(%esp),%ebx
	movl	12(%esp),%eax
	xchgl	%eax,(%ebx)
	popl	%ebx
	ret
	
	/*
		//PROLOGUE
		pushl	%ebp		 //Push ebp onto stack
		movl	%esp,%ebp	 //Record current SP in ebp

		//Pushing the general purpose registers on stack
		pushl 	%ebx		//pushing ebx for temp storage		
	
		subl	$4,%esp		//creating space to store variable 'old value'
		xchg	8(%ebp),%ebx	//copying the pointer to ebx...............................
	
		//next 2 lines are done in 2 instructions because we cant have 2 memory referneces in one instruction
		xchg	(%ebx),%eax	//exchanging the contents of pointer dereference and eax
		xchg	%eax,-8(%ebp)	//exchanging the contents of eax with 'old value' ... at this point old value = *ptr 

		xchg	12(%ebp),%eax	//copying the new value into eax now...............................
		xchg	%eax,(%ebx)	//exchanging the contents of eax with pointer dereference..now *ptr = new_value
	
		xchg	-8(%ebp),%eax	//finally putting the value to be returned into eax

		//Popping values begin
		addl	$4,%esp		//esp now points to ebx
		
		//EPILOGUE
		popl	%ebx		//popping ebx
		movl	%ebp,%esp	//making sure ebp and esp point to same location...just for correctness check..not required though
		popl 	%ebp		//popping ebp 
		ret
	*/
	
	
