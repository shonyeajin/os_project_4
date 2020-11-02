#include "rw_lock.h"
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_read = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_write = PTHREAD_COND_INITIALIZER;



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
		//write lock 걸려있으면 대기
		if(rw->write_type==1){
				rw->num_read_waits++;
				pthread_cond_wait(&cond_read,&mutex);
				pthread_mutex_unlock(&mutex);
				rw->num_read_waits--;
		}
		rw->read_type=1;
		rw->read_state++;


}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
		rw->read_state--;
		if(rw->read_state==0&&rw->num_read_waits==0){
				rw->read_type=0;
				if(rw->num_write_waits>0)
						pthread_cond_signal(&cond_write);
				pthread_mutex_unlock(&mutex);
		}
}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
		usleep(100000);
		if(rw->read_type==1||rw->write_type==1/*||rw->num_read_waits>0*/){
				rw->num_write_waits++;
				pthread_cond_wait(&cond_write,&mutex);
				rw->num_write_waits--;
		}
		rw->write_type=1;
		rw->write_state++;

}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
		rw->write_state--;
		rw->write_type=0;

		if(rw->num_read_waits>0){
				pthread_mutex_unlock(&mutex);
				pthread_cond_broadcast(&cond_read);
		}else if(rw->num_write_waits>0){
				pthread_mutex_unlock(&mutex);
				pthread_cond_signal(&cond_write);
		}





}
