#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct rw_lock
{
		int read_type;
		int write_type;
		int num_read_waits;
		int num_write_waits;
		int read_state;
		int write_state;




};

void init_rwlock(struct rw_lock * rw);
void r_lock(struct rw_lock * rw);
void r_unlock(struct rw_lock * rw);
void w_lock(struct rw_lock * rw);
void w_unlock(struct rw_lock * rw);
long *max_element(long* start, long* end);
long *min_element(long* start, long* end);
