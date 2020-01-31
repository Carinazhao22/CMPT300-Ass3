#include "scheduler.h"
#include "sched_impl.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

 

static void init_thread_info(thread_info_t *info, sched_queue_t *queue)
{
	info->queue = queue;
	info->element=NULL;

    sem_init(&info->cpu, 0, 0);
	 
}

 
static void destroy_thread_info(thread_info_t *info)
{
	 
	info->queue = NULL; 
	info->element = NULL;
	 
	sem_destroy(&info->cpu);
	 
}

 
static void enter_sched_queue(thread_info_t *info)
{
	 
	sem_wait(&info->queue->admission); //admission--;
	pthread_mutex_lock(&info->queue->lock);

	info->element=malloc(sizeof(list_elem_t));
	list_elem_init(info->element,info);
	list_insert_tail(info->queue->q, info->element);

	pthread_mutex_unlock(&info->queue->lock);
	 
}

 
static void leave_sched_queue(thread_info_t *info)
{
	 
	pthread_mutex_lock(&info->queue->lock);
	
	list_remove_elem(info->queue->q, info->element);
	free(info->element);
	pthread_mutex_unlock(&info->queue->lock);
	sem_post(&info->queue->admission);//admission++;

	 
}

 
static void wait_for_cpu(thread_info_t * info)
{
  sem_wait(&info->cpu);//cpu=0
 
}

 
static void release_cpu(thread_info_t * info)
{
	 
	sem_post(&info->queue->ready); //ready--;
}

 //scheduler implementation

 
static void init_sched_queue(sched_queue_t *queue, int queue_size)
{
	 
 
	queue->q = (list_t *) malloc(sizeof(list_t));
	list_init(queue->q);
	//used for rr
	queue->curr = 0;
	queue->temp = 0;


    pthread_mutex_init(&queue->lock,NULL);
	sem_init(&queue->ready, 0, 0);
	sem_init(&queue->admission, 0, queue_size);

}

 
static void destroy_sched_queue(sched_queue_t *queue)
{
	 list_elem_t *del;
	while(queue->q->head!=NULL)
	{
		del=list_get_head(queue->q);
		list_remove_elem(queue->q, del);
		free(del);
	}

	
	free(queue->q);	
	 
	sem_destroy(&queue->ready);
	sem_destroy(&queue->admission);
    pthread_mutex_destroy(&queue->lock);
}

 
static void wake_up_worker(thread_info_t *info)
{
	 
	sem_post(&info->cpu);//cpu=1
	 
}

 
static void wait_for_worker(sched_queue_t *queue)
{
	 sem_wait(&queue->ready); //ready--;
}
 
static thread_info_t * next_worker_fifo(sched_queue_t *queue)
{
 
	list_elem_t *temp = list_get_head(queue->q);
	 if(list_size(queue->q)==0)
	 return NULL;
	 else
	 {
	  thread_info_t *nextWorker = (thread_info_t*) (temp->datum);
		return nextWorker;
	 
	 }
		
}

 
static thread_info_t * next_worker_rr(sched_queue_t *queue)
{

	list_elem_t *head;
    thread_info_t *worker;

int size=list_size(queue->q);
   
 if(size==0)
      return NULL;


 if(queue->curr!=0&&queue->temp==0)
    queue->curr=size;
   
if(queue->curr<=size)
	{
	
	queue->curr=size;
	queue->temp=size;
     
	 
      pthread_mutex_lock(&queue->lock);
      head = list_get_head(queue->q);
	 
      list_remove_elem(queue->q, head);
      list_insert_tail(queue->q, head);
      head = list_get_head(queue->q);
     pthread_mutex_unlock(&queue->lock);
      
   }
 else
   {
	   pthread_mutex_lock(&queue->lock);
	   head = list_get_head(queue->q);
	   pthread_mutex_unlock(&queue->lock);
	   queue->temp--;
	}

   worker = (thread_info_t*) head->datum;
   return worker; 
}

 
static void wait_for_queue(sched_queue_t *queue)
{
	//wait until there are some worker into the queue
	int curr_size;
	do{
		curr_size=list_size(queue->q);
       }while(curr_size==0);
	 
 
 
}


/*...More functions go here...*/
/* You need to statically initialize these structures: */
sched_impl_t sched_fifo = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu,release_cpu }, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker,wait_for_worker, next_worker_fifo, wait_for_queue} },
sched_rr = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu,release_cpu }, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker,wait_for_worker, next_worker_rr, wait_for_queue} };


 