#include <xinu.h>

uint8 allocation[NPROC][MAX_LOCKS];
uint8 request[NPROC][MAX_LOCKS];
uint8 available[MAX_LOCKS] = {1};
uint8 work[MAX_LOCKS] = {0};
uint8 finish[NPROC] = {0};

uint8 global_counter = 0;

al_lock_t l0;
al_lock_t l1;
al_lock_t l2;
al_lock_t l3;

al_lock_t l4;
al_lock_t l5;
al_lock_t l6;
al_lock_t l7;

void p3_deadlock1(){
  al_lock(&l0);
  //kprintf("\nLock 0 acquired by deadlock 1");
  sleepms(1);
  al_lock(&l1);
  kprintf("\nLock 1 acquired by deadlock 1");

  //critical section in theory

  al_unlock(&l1);
  kprintf("\nLock 1 released by deadlock 1");
  al_unlock(&l0);
  kprintf("\nLock 0 released by deadlock 1");
}

void p3_deadlock2(){
  al_lock(&l1);
  //kprintf("\nLock 1 acquired by deadlock 2");
  sleepms(1);
  al_lock(&l2);
  kprintf("\nLock 2 acquired by deadlock 2");

  //critical section in theory

  al_unlock(&l2);
  kprintf("\nLock 2 released by deadlock 2");
  al_unlock(&l1);
  kprintf("\nLock 1 released by deadlock 2");
}

void p3_deadlock3(){
  al_lock(&l2);
  //kprintf("\nLock 2 acquired by deadlock 3");
  sleepms(1);
  al_lock(&l3);
  kprintf("\nLock 3 acquired by deadlock 3");

  //critical section in theory

  al_unlock(&l3);
  kprintf("\nLock 3 released by deadlock 3");
  al_unlock(&l2);
  kprintf("\nLock 2 released by deadlock 3");
}

void p3_deadlock4(){
  al_lock(&l3);
  //kprintf("\nLock 3 acquired by deadlock 4");
  sleepms(1);
  al_lock(&l0);
  kprintf("\nLock 0 acquired by deadlock 4");

  //critical section in theory

  al_unlock(&l0);
  kprintf("\nLock 0 released by deadlock 4");
  al_unlock(&l3);
  kprintf("\nLock 3 released by deadlock 4");
}


void trylock1(){
 start1:
  //kprintf("\nhere for 1");
  if(al_trylock(&l4) == 0){
    goto start1;
  }
  else{
    sleepms(1);
    if(al_trylock(&l5) == 0){
      al_unlock(&l4);
      goto start1;
    }

    //kprintf("\nLock 1 acquired by deadlock 1");

    //critical section in theory

    al_unlock(&l5);
    //kprintf("\nLock 1 released by deadlock 1");
    al_unlock(&l4);
    //kprintf("\nLock 0 released by deadlock 1");
  }
}

void trylock2(){
  start2:
  //kprintf("\nhere for 2");
  if(al_trylock(&l5) == 0){
    goto start2;
  }
  else{
    sleepms(1);
    if(al_trylock(&l6) == 0){
      al_unlock(&l5);
      goto start2;
    }

    //kprintf("\nLock 2 acquired by deadlock 2");

    //critical section in theory

    al_unlock(&l6);
    //kprintf("\nLock 2 released by deadlock 2");
    al_unlock(&l5);
    //kprintf("\nLock 1 released by deadlock 2");
  }
}

void trylock3(){
start3:
  //kprintf("\nhere for 3");
  if(al_trylock(&l6) == 0){
    goto start3;
  }
  else{
    sleepms(1);
    if(al_trylock(&l7) == 0){
      al_unlock(&l6);
      goto start3;
    }

    //kprintf("\nLock 3 acquired by deadlock 3");

    //critical section in theory

    al_unlock(&l7);
    //kprintf("\nLock 3 released by deadlock 3");
    al_unlock(&l6);
    //kprintf("\nLock 2 released by deadlock 3");
  }
}

void trylock4(){
  start4:
  //kprintf("\nhere for 4");
  if(al_trylock(&l7) == 0){
    goto start4;
  }
  else{
    sleepms(1);
    if(al_trylock(&l4) == 0){
      al_unlock(&l7);
      goto start4;
    }

    //kprintf("\nLock 0 acquired by deadlock 4");

    //critical section in theory

    al_unlock(&l4);
    //kprintf("\nLock 0 released by deadlock 4");
    al_unlock(&l7);
    //kprintf("\nLock 3 released by deadlock 4");
  }
}

void main_deadlock(){  
  al_init(&l0);
  al_init(&l1);
  al_init(&l2);
  al_init(&l3);

  resume(create(p3_deadlock1, 4096, 15, "DD", 0, NULL));
  resume(create(p3_deadlock2, 4096, 15, "DD", 0, NULL));
  resume(create(p3_deadlock3, 4096, 15, "DD", 0, NULL));
  resume(create(p3_deadlock4, 4096, 15, "DD", 0, NULL));
  
}

void main_trylock(){  
  al_init(&l4);
  al_init(&l5);
  al_init(&l6);
  al_init(&l7);
 
  resume(create(trylock1, 4096, 15, "DD", 0, NULL));
  resume(create(trylock2, 4096, 15, "DD", 0, NULL));
  resume(create(trylock3, 4096, 15, "DD", 0, NULL));
  resume(create(trylock4, 4096, 15, "DD", 0, NULL));


}

