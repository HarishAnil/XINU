#include <xinu.h>

int i_naive = 0;
int i_lock = 0;
int done_naive = 0;
int done_lock = 0;

uint64 naive_parallel_sum = 0;
uint64 sync_parallel_sum = 0;

bwf_lock_t bwf;

void summation(uint32 * array, uint32 n){

  while(i_naive < n){
    naive_parallel_sum += *array;
    array++;
    ++i_naive;
  }
  
  done_naive++;
}

void lock_summation(uint32 * array, uint32 n){  
  
  uint32 local_index;
  do{
    bwf_lock(&bwf);
    local_index = i_lock++;
    bwf_unlock(&bwf);
    
    if(local_index < n){
      bwf_lock(&bwf);
      sync_parallel_sum += *array;
      array++;
      bwf_unlock(&bwf);
    }
    
  }while(local_index < n);

  
  done_lock++;
}

uint32 serial_summation(uint32 *array,uint32 n){
  uint64 i,sum = 0;
  for(i=0; i<n; i++){
    sum += *array;
    array++;
  }

  return sum;
}


uint32 naive_parallel_summation(uint32 *array,uint32 n, uint32 num_threads){
  int i;

  for(i=0; i<num_threads; i++){
    resume(create(summation, 1024, 15, "Proc", 2, array,n));
  }

  while(done_naive<num_threads)
    sleepms(50);
  
  return naive_parallel_sum;
}

uint32 sync_parallel_summation(uint32 *array,uint32 n, uint32 num_threads){
  int i;
  uint32 *p3_array = array;
  
  bwf_init(&bwf);
  //creating processes for p3
  for(i=0; i<num_threads; i++){
    resume(create(lock_summation, 1024, 15, "Lock", 2, p3_array,n));
  }
  //wait till p3 is complete  
  while(done_lock<num_threads)
    sleepms(50);

  return sync_parallel_sum;
}

void main_basic(){
  uint32 array_size = 900000,num_threads = 30;
  uint32 i;
  uint32 *myarray = (uint32*)getmem(sizeof(uint32)*array_size);
  uint64 sum = 0;

  for(i=0 ;i<array_size; i++)
    myarray[i] = 1;

  uint32 *array = myarray;
  uint32 *naive_array = myarray;
  uint32 *lock_array = myarray;

  sum = serial_summation(array,array_size);
  printf("Serial sum is: %d\n", sum);

  sum=0;
  sum = naive_parallel_summation(naive_array,array_size,num_threads);
  printf("Naive parallel sum is %d\n",sum);

  sum=0;
  sum = sync_parallel_summation(lock_array,array_size,num_threads);
  printf("Locked parallel sum with busy wait free is %d\n", sum);

  freemem((char*)myarray,sizeof(uint32)*array_size);

}
