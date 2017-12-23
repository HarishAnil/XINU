#include <xinu.h>

void sl_init(sl_lock_t *l){
  l->flag = 0;
}

void sl_lock(sl_lock_t *l){
  while(test_and_set(&l->flag,1)==1)
    ;
}

void sl_unlock(sl_lock_t *l){
  l->flag = 0;
}




