//#include<semaphore.h>
//#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define thinking 0
#define hungry 1
#define eating 2

int n;
int *state;
int *phil;

omp_lock_t lock1;  //to prevent multiple threads from changing the state at the same time
omp_lock_t *phil_lock;

void rand_sleep()
{
	int time= rand()%10;
	//printf("Idle for %d sec\n",time);
	
	usleep(100);
}

void eat_test(int philn)
{
  int left= ((philn-1)<0)?n-1:philn-1;
  int right= ((philn+1)>n-1)?0:philn+1;
  //sem_wait(&lock1);
  //do{
  if(state[philn]==hungry && state[left]!=eating && state[right]!=eating)
  {
    
    state[philn]=eating;
    printf("Philosopher %d takes fork %d and %d and is eating\n",philn+1,left+1,philn+1);
    
    rand_sleep();
    omp_unset_lock(&phil_lock[philn]);
    
  }
  //}while(state[philn]!=eating);
  //sem_post(&lock1);
}

void* philosopher()
{
  int philn= omp_get_thread_num();
  int left= ((philn-1)<0)?n-1:philn-1;
  int right= ((philn+1)>n-1)?0:philn+1;

  omp_set_lock(&phil_lock[philn]);
  int loopi=30;
  while((loopi--)>=0){
    int hun=1;
    //thinking:
    printf("Philosopher %d is thinking\n",philn+1);
    rand_sleep();
    //hungry:
   // printf("Hungry!!!!\n");
    sleep(1);
    omp_set_lock(&lock1);
    //printf("test\n");
    state[philn] = hungry ;
    
    printf("Philosopher %d is hungry \n",philn+1);
    //sem_post(&lock1);  
    omp_unset_lock(&lock1);
    eat_test(philn);
    
    omp_set_lock(&phil_lock[philn]);
    
    //finished eating
    omp_set_lock(&lock1);
    state[philn]=thinking;
    printf("Philosopher %d puts down fork %d and %d\n",philn+1,left+1, philn+1);
    eat_test(left);
    eat_test(right);
    omp_unset_lock(&lock1);
  }
}

int main(int argc, char *argv[])
{
  int i;
  if(argc!=2)
  {
    printf("Usage: ./phil <number of philosophers>\n");
    return 1;
  } 
  n= atoi(argv[1]);
  printf("%d\n",n);
  state = (int *)malloc(n*(sizeof(int)));
  phil= (int *) malloc(n*(sizeof(int)));
  phil_lock = (omp_lock_t *) malloc(n*(sizeof(omp_lock_t)));
  
  //pthread_t thread_id[n];
  //initializing phil
  for(i=0;i<n;i++)
  {phil[i]=i;
    state[i]=0;
    }
  //initializing sem_lock and lock
  omp_init_lock(&lock1);    //initially unlocked
  //sem_init(&lock2,0,1);
  //sem_init(&lock3,0,1);
  for(i=0;i<n;i++)
  omp_init_lock(&phil_lock[i]);    //initially locked
  
  //creating threads
  /*
  for(i=0;i<n;i++)
    pthread_create(&thread_id[i],NULL, philosopher, &phil[i]);
  for(i=0;i<n;i++)
    pthread_join(thread_id[i], NULL);
    */
   
#pragma omp parallel num_threads(n)
  {
      philosopher();
  }

  omp_destroy_lock(&lock1);
  for(i=0;i<n;i++)
      omp_destroy_lock(&phil_lock[i]);

  return 0;
}
  
