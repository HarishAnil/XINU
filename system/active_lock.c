#include <xinu.h>

void dd_step2(void);

void al_init(al_lock_t *l){
  l->flag = 0;
  l->index = global_counter++;
  l->wait_queue = newqueue();
}

void al_lock(al_lock_t *l){
  intmask mask;
  while(test_and_set(&l->flag,1)==1){
    request[currpid][l->index] = 1;
    deadlock_detector();
    //Add process to the queue if lock unavailable
    enqueue(getpid(), l->wait_queue);
    //Make the process WAITING as it waits for the lock
    proctab[currpid].prstate = PR_WAIT;
    //kprintf("Putting %s process in Waiting\n", proctab[currpid].prname);
    mask = disable();
    resched();
    restore(mask);
  }
  request[currpid][l->index] = 0;
  allocation[currpid][l->index] = 1;
  available[l->index] = 0;
}


void al_unlock(al_lock_t *l){
  l->flag = 0;
  allocation[currpid][l->index] = 0;
  available[l->index] = 1;

  pid32 pid;
  if(nonempty(l->wait_queue)){
    pid = dequeue(l->wait_queue);
    if(isbadpid(pid))
      kprintf("Bad pid\n");

      ready(pid);
  }
}

bool8 al_trylock(al_lock_t *l){
  if(test_and_set(&l->flag,1)==1)
    return 0;
  else
    return 1;
}

void dd_init(){
  int i,j,x;
  for(i=0; i<MAX_LOCKS; i++){
    work[i] = available[i];
  }

  for(i=0; i<NPROC; i++){
    x = 0;
    for(j=0; j<MAX_LOCKS; j++){
      if(allocation[i][j] == 1){
	x=1;
	break;
      }
    }
    if(x == 1)
      finish[i] = 0;
    else
      finish[i] = 1;
  }
}

void dd_step3(uint8 *i){
  uint8 j;
  for(j=0; j<MAX_LOCKS; j++)
    work[j] += allocation[(*i)][j];

  finish[(*i)] = 1;
  dd_step2();
}

void dd_final_step(){
  uint8 i,x=0;
  for(i=0; i<NPROC; i++){
    if(finish[i] == 0){
      x = 1;
      break;
    }
  }

  if(x == 1)
    kprintf("\n!!DEADLOCK DETECTED\n");
}

void dd_step2(){
  uint8 i,j,x,y;
  
  for(i=0; i<NPROC; i++){
    x = 0;
    if(finish[i] == 0){
      for(j=0; j<MAX_LOCKS; j++){
	y = 0;
	if(request[i][j]>work[j]){
	  y = 1;
	  break;
	}
      }

      if(y == 1) 
	continue;

      else{
	x = 1; // found the index
	dd_step3(&i);
      }
    }
  }

  if(x == 0)
    dd_final_step();
}

void deadlock_detector(){
  dd_init();
  dd_step2();
  
}
