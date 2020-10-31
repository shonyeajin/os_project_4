#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>


int item_to_produce, curr_buf_size, item_to_consume;
int total_items, max_buf_size, num_workers, num_masters;

pthread_mutex_t mutex_master=PTHREAD_MUTEX_INITIALIZER;//쓰레드 초기화
pthread_cond_t cond_master=PTHREAD_COND_INITIALIZER;//cond 초기화
pthread_cond_t cond_worker=PTHREAD_COND_INITIALIZER;//cond 초기화

int *buffer;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
    {

	  pthread_mutex_lock(&mutex_master);//master끼리의 lock
			//버퍼 꽉차면 기달
			//아이템 다 만들면 종료
      if(item_to_produce >= total_items) {
			  pthread_mutex_unlock(&mutex_master);
			  pthread_cond_broadcast(&cond_worker);
			  break;
      }
	  if(curr_buf_size==max_buf_size){
			  pthread_cond_broadcast(&cond_worker);
			  pthread_cond_wait(&cond_master,&mutex_master);
			  
		      if(item_to_produce >= total_items) {
				  pthread_mutex_unlock(&mutex_master);
				  pthread_cond_broadcast(&cond_worker);
				  break;
			  }


	  }

 


      buffer[curr_buf_size++] = item_to_produce;
      print_produced(item_to_produce, thread_id);
      item_to_produce++;

	  pthread_mutex_unlock(&mutex_master);//master끼리의 unlock

    }
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

void *consume_requests_loop(void *data)
{
		int thread_id=*((int *)data);

		while(1)
		{


				pthread_mutex_lock(&mutex_master);
				//아이템 다 소비하면 종료
				//버퍼 비어있으면 wait
				if(item_to_consume<=0){
						pthread_mutex_unlock(&mutex_master);
						break;
				}

				if(curr_buf_size<=0){
						pthread_cond_broadcast(&cond_master);
						pthread_cond_wait(&cond_worker,&mutex_master);

						if(item_to_consume<=0){
								pthread_mutex_unlock(&mutex_master);
								break;
						}


				}
				print_consumed(buffer[--curr_buf_size],thread_id);
				item_to_consume--;
				pthread_mutex_unlock(&mutex_master);



				

		}

		return 0;
}


int main(int argc, char *argv[])
{
  int *master_thread_id;
  pthread_t *master_thread;

  int *worker_thread_id;
  pthread_t *worker_thread;
 
  item_to_produce = 0;
  curr_buf_size = 0;

  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
	item_to_consume=total_items;
  }
    

   buffer = (int *)malloc (sizeof(int) * max_buf_size);



   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);//마스터 스레드 아이디 메모리 할당
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);//마스터 스레드 자체 메모리 할당
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;//마스터 스레드에 0부터 아이디 할당

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);//마스터 스레드 공간에 tid값 할당, 정수만드는 loop수행
  
  //create worker consumer threads
  worker_thread_id=(int *)malloc(sizeof(int) * num_workers);
  worker_thread=(pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for(i=0;i<num_workers;i++)
		  worker_thread_id[i]=i;
  for(i=0;i<num_workers;i++)
		  pthread_create(&worker_thread[i],NULL,consume_requests_loop,(void *)&worker_thread_id[i]);



  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
    {
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
    }
   for (i = 0; i < num_workers; i++)
    {
      pthread_join(worker_thread[i], NULL);
      printf("worker %d joined\n", i);
    }
  
   pthread_mutex_destroy(&mutex_master);
   pthread_cond_destroy(&cond_master);
   pthread_cond_destroy(&cond_worker);
  
  /*----Deallocating Buffers---------------------*/

  free(worker_thread);
  free(worker_thread_id);
  free(master_thread);
  free(master_thread_id);
  free(buffer);

  
  return 0;
}
