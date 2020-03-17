//Dining philosophers problem showcasing even number of philosophers (extra credit 1)

#include<semaphore.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#define thinking 0
#define hungry 1
#define eating 2

int n;
int *state;
int *phil;

sem_t lock1;  //to prevent multiple threads from changing the state at the same time
sem_t *phil_lock;   //locks for each philosopher 

//Generating random time
void rand_sleep()
{
	int time= rand()%10;
	//printf("Idle for %d sec\n",time);
	
	sleep(time);
}

//Testing if the philosopher is eating
void eat_test(int philn)
{
  int left= ((philn-1)<0)?n-1:philn-1;
  int right= ((philn+1)>n-1)?0:philn+1;
  //sem_wait(&lock1);
  //do{
  //checking if nearby forks are free
  if(state[philn]==hungry && state[left]!=eating && state[right]!=eating)
  {
    
    state[philn]=eating;
    printf("Philosopher %d takes fork %d and %d and is eating\n",philn+1,left+1,philn+1);
    
    rand_sleep();
    sem_post(&phil_lock[philn]);    //releasing the lock when the philosopher finishes eating
  
  }
  //}while(state[philn]!=eating);
  //sem_post(&lock1);
}

//This function is run parallelly
void* philosopher(void* i)
{
  int philn= *(int *)i;
  int left= ((philn-1)<0)?n-1:philn-1;
  int right= ((philn+1)>n-1)?0:philn+1;

  while(1){
    int hun=1;
    //thinking:
    printf("Philosopher %d is thinking\n",philn+1);
    rand_sleep();
    //hungry:
    //printf("Hungry!!!!\n");
    sleep(1);
    sem_wait(&lock1);
    //printf("test\n");
    state[philn] = hungry ;
    
    printf("Philosopher %d is hungry \n",philn+1);
    sem_post(&lock1);  
    eat_test(philn);
    
    sem_wait(&phil_lock[philn]);
    
    //finished eating
    sem_wait(&lock1);
    state[philn]=thinking;
    printf("Philosopher %d puts down fork %d and %d\n",philn+1,left+1, philn+1);
    // Testing left and right philosophers if they can eat
    eat_test(left);
    eat_test(right);
    sem_post(&lock1);
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
  //Dynamically allocating space for state, phil and phil_lock based on number of threads
  state = (int *)malloc(n*(sizeof(int)));
  phil= (int *) malloc(n*(sizeof(int)));
  phil_lock = (sem_t *) malloc(n*(sizeof(sem_t)));
  
  pthread_t thread_id[n];
  //initializing phil
  for(i=0;i<n;i++)
  {phil[i]=i;
    state[i]=0;
    }
  //initializing sem_lock and lock
  sem_init(&lock1,0,1);    //initially unlocked
  for(i=0;i<n;i++)
  sem_init(&phil_lock[i],0,0);    //initially locked
  
  //creating threads
  for(i=0;i<n;i++)
    pthread_create(&thread_id[i],NULL, philosopher, &phil[i]);

  //Joining Threads
  for(i=0;i<n;i++)
    pthread_join(thread_id[i], NULL);
    
}
  
