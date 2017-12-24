#include <xinu.h>

status_t insert_into_llist(struct shared_mem *linkedlist, struct shared_mem *s){

	struct shared_mem *llist = proctab[getpid()].shared_heap_table;

	if(llist->next == NULL){
		llist->next = s;
		kprintf("Owner : %d\n",proctab[getpid()].shared_heap_table->next->shared_heap_owner);
	} else {

		while(llist->next != NULL)
			llist = llist->next;
		llist->next = s;
	}

	return OK;
}
