#ifndef	__SCHED_IMPL__H__
#define	__SCHED_IMPL__H__

#include "list.h"
#include "scheduler.h"
#include <pthread.h>
#include <semaphore.h>

struct thread_info {
	/*...Fill this in...*/
  sched_queue_t* queue;
  list_elem_t* element;
  sem_t cpu;
   

};

struct sched_queue {
/*...Fill this in...*/
 list_t* q;   
 sem_t ready,admission;
 pthread_mutex_t lock;
 int curr;
 int temp;


};

#endif /* __SCHED_IMPL__H__ */
 

 