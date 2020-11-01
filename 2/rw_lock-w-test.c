#include "rw_lock.h"
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_read=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_write=PTHREAD_COND_INITIALIZER;

void init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
		rw->read_type=0;
		rw->write_type=0;
		rw->num_read_waits=0;
		rw->num_write_waits=0;
		rw->read_state=0;
		rw->write_state=0;


}

void r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
		if(rw->write_type==1||rw->num_write_waits>0){//write락이 걸려있으면 read락 못걸고 기달
				rw->num_read_waits++;
				pthread_cond_wait(&cond_read,&mutex);
				pthread_mutex_unlock(&mutex);
				rw->num_read_waits--;
		}

		//write락 없으면
		//기달리는 write락 없으면 계속 read락 수용
		//기달리는 write락 있으면 read락도 더이상 read락 못걸게
		/*
		if(rw->num_write_waits>0){
				rw->read_waits++;
				pthread_cond_wait(&cond_read,&mutex);
				rw->read_waits--;
		}
		*/

		rw->read_type=1;
		rw->read_state++;

}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
		rw->read_state--;
		//pthread_mutex_unlock(&mutex);

		if(rw->read_state==0){
				rw->read_type=0;
				if(rw->num_write_waits>0)
						pthread_cond_signal(&cond_write);
				pthread_mutex_unlock(&mutex);
		}

}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
		//readlock이 걸려있는지 확인
		if(rw->read_type==1||rw->write_type==1){
				rw->num_write_waits++;
				pthread_cond_wait(&cond_write,&mutex);
				//printf("writer 깨워졋ㅆ음\n");
				rw->num_write_waits--;
		}

		//writelock도 걸려있으면 기달려야함.
		/*
		if(rw->write_type==1){
				rw->write_waits++;
				pthread_cond_wait(&cond_write,&mutex);
				rw->write_waits--;
		}
		*/

		//read락,write락이 안걸려 있으면 write락 건다.
		rw->write_type=1;
		rw->write_state++;



}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
		//기다리는 다른 write락이 있으면 개를 깨워줌
		//리드락만 기다리고 있으면 개네들 깨워줌
		rw->write_state--;
		rw->write_type=0;
		

		if(rw->num_write_waits>0){
				//printf("writer 깨웠음;\n");
				pthread_mutex_unlock(&mutex);
				pthread_cond_signal(&cond_write);
		}else if(rw->num_read_waits>0){
				pthread_mutex_unlock(&mutex);
				pthread_cond_broadcast(&cond_read);
		}





}
