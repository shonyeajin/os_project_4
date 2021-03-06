#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "SSU_Sem.h"

#define NUM_THREADS 3
#define NUM_ITER 10
/*
SSU_Sem first;
SSU_Sem second;
SSU_Sem third;
*/
SSU_Sem *SEM[NUM_THREADS];

void *justprint(void *data)
{
  int thread_id = *((int *)data);
  

  for(int i=0; i < NUM_ITER; i++)
    {
			/*
			switch(thread_id){
					case 0:
							SSU_Sem_down(SEM[0]);
							break;
					case 1:
							SSU_Sem_down(SEM[1]);
							break;
					case 2:
							SSU_Sem_down(SEM[2]);
							break;
			}
			*/
			for(int j=0;j<NUM_THREADS;j++){
					if(thread_id==j)
							SSU_Sem_down(SEM[j]);
			}

      printf("This is thread %d\n", thread_id);
	  /*

			switch(thread_id){
					case 0:
							SSU_Sem_up(SEM[1]);
							break;
					case 1:
							SSU_Sem_up(SEM[2]);
							break;
					case 2:
							SSU_Sem_up(SEM[0]);
							break;
			}
			*/
			for(int j=0;j<NUM_THREADS;j++){
					if(thread_id==j)
							SSU_Sem_up(SEM[(j+1)%NUM_THREADS]);
			}




	  
    }
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];
/*
  SSU_Sem_init(&first,0);
  SSU_Sem_init(&second,0);
  SSU_Sem_init(&third,0);
*/
  for(int i=0;i<NUM_THREADS;i++){
		  SEM[i]=(SSU_Sem *)malloc(sizeof(SSU_Sem));
		  SSU_Sem_init(SEM[i],0);
  }


  
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
    }

  usleep(100000);
  SSU_Sem_up(SEM[0]);
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }

  for(int i=0;i<NUM_THREADS;i++){
		  free(SEM[i]);
  }
  free(*SEM);

  return 0;
}
