 #include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "scheduler.h"

 
sigset_t mask;
/*******************************************************************************
 *
 * Implement these functions.
 *
 ******************************************************************************/

/* Handler for SIGTERM signal */
void cancel_thread()
{ 
	printf("Thread %u: terminating.\n", (unsigned int)pthread_self());

	/* signal that done in queue */
	sem_post(&queue_sem);

	pthread_exit(NULL);
} 

/* TODO: Handle the SIGUSR1 signal */
void suspend_thread()
{   
 int n;
    
	printf("Thread %u: suspending.\n", (unsigned int)pthread_self());
	/*add your code here to wait for a resume signal from the scheduler*/
    sigwait(&mask,&n);

	printf("Thread %u: resuming.\n",(unsigned int) pthread_self());
}

/*******************************************************************************
 *
 * 
 *
 ******************************************************************************/
/*
 * waits to gain access to the scheduler queue.
 */
static int enter_scheduler_queue(thread_info_t *info)
{
	/*
	 * wait for available room in queue.
	 * create a new list entry for this thread
	 * store this thread info in the new entry.
	 */
	sem_wait(&queue_sem);
	list_elem *item = (list_elem*)malloc(sizeof(list_elem));
	info->le = item;
	item->info = info;
	item->prev = 0;
	item->next = 0;
	list_insert_tail(&sched_queue, item);
	return 0;
}

/*
 * leaves the scheduler queue
 */
void leave_scheduler_queue(thread_info_t *info)
{
	printf("Thread %lu: leaving scheduler queue.\n", info->thrid);
	/*
	 * remove the given worker from queue
	 * clean up the memory that we malloc'd for the list
	 * clean up the memory that was passed to us
	 */
	list_remove(&sched_queue, info->le);
	free(info->le);
	free(info);
}


/*
 * Initialize thread, enter scheduling queue, and execute instructions.
 * arg is a pointer to thread_info_t
 */
void *start_worker(void *arg)
{
	thread_info_t *info = (thread_info_t *) arg;
	float calc = 0.8;
	int j = 0;
    // sigset_t Bmask, Umask,mask;
	sigset_t Bmask, Umask;

	sigemptyset(&Bmask);
	sigemptyset(&Umask);
	sigemptyset(&mask);
 

	
	/* TODO: Block SIGALRM and SIGUSR2. */
	 sigaddset(&Bmask, SIGALRM);
	 sigaddset(&mask, SIGUSR2);
	 pthread_sigmask(SIG_BLOCK,&Bmask,NULL);
	 pthread_sigmask(SIG_BLOCK,&mask,NULL);
	  
	 
	/* TODO: Unblock SIGUSR1 and SIGTERM. */
     
	 sigaddset(&Umask, SIGUSR1);
	 sigaddset(&Umask, SIGTERM);
     pthread_sigmask(SIG_UNBLOCK,&Umask,NULL);


	/* compete with other threads to enter queue. */
	if (enter_scheduler_queue(info)) {
		printf("Thread %lu: failure entering scheduler queue - %s\n", info->thrid, strerror(errno));
		free (info);
		pthread_exit(0);
	}
	printf("Thread %lu: in scheduler queue.\n", info->thrid);

	suspend_thread();

	while (1) {
		/* do some meaningless work... */
		for (j = 0; j < 10000000; j++) {
			calc = 4.0 * calc * (1.0 - calc);
		}
	}
}
