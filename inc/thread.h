#ifndef __THREAD_H__
#define __THREAD_H__


#include <pthread.h>
#include <semaphore.h>



/* ================================= STRUCTURES ================================================ */


/* Individual job */
typedef struct thpool_job_t{
	void*  (*function)(void* arg);                     /**< function pointer         */
	void*                     arg;                     /**< function's argument      */
	struct thpool_job_t*     next;                     /**< pointer to next job      */
	struct thpool_job_t*     prev;                     /**< pointer to previous job  */
}thpool_job_t;


/* Job queue as doubly linked list */
typedef struct thpool_jobqueue{
	thpool_job_t *head;                                /**< pointer to head of queue */
	thpool_job_t *tail;                                /**< pointer to tail of queue */
	int           jobsN;                               /**< amount of jobs in queue  */
	sem_t        *queueSem;                            /**< semaphore(this is probably just holding the same as jobsN) */
}thpool_jobqueue;


/* The threadpool */
typedef struct thpool_t{
	pthread_t*       threads;                          /**< pointer to threads' ID   */
	int              threadsN;                         /**< amount of threads        */
	thpool_jobqueue* jobqueue;                         /**< pointer to the job queue */
}thpool_t;


/* Container for all things that each thread is going to need */
typedef struct thread_data{                            
	pthread_mutex_t *mutex_p;
	thpool_t        *tp_p;
}thread_data;

class CThread
{

public:

	CThread();

	~CThread(){};

public:


	/**
	* @brief  Initialize threadpool
	* 
	* Allocates memory for the threadpool, jobqueue, semaphore and fixes 
	* pointers in jobqueue.
	* 
	* @param  number of threads to be used
	* @return threadpool struct on success,
	*         NULL on error
	*/
	static thpool_t* thpool_init(int threadsN);


	/**
	* @brief What each thread is doing
	* 
	* In principle this is an endless loop. The only time this loop gets interuppted is once
	* thpool_destroy() is invoked.
	* 
	* @param threadpool to use
	* @return nothing
	*/
	static void* thpool_thread_do(void* tp);


	/**
	* @brief Add work to the job queue
	* 
	* Takes an action and its argument and adds it to the threadpool's job queue.
	* If you want to add to work a function with more than one arguments then
	* a way to implement this is by passing a pointer to a structure.
	* 
	* ATTENTION: You have to cast both the function and argument to not get warnings.
	* 
	* @param  threadpool to where the work will be added to
	* @param  function to add as work
	* @param  argument to the above function
	* @return int
	*/
	static int thpool_add_work(thpool_t* tp_p, void *(*function_p)(void*), void* arg_p);


	/**
	* @brief Destroy the threadpool
	* 
	* This will 'kill' the threadpool and free up memory. If threads are active when this
	* is called, they will finish what they are doing and then they will get destroyied.
	* 
	* @param threadpool a pointer to the threadpool structure you want to destroy
	*/
	static void thpool_destroy(thpool_t* tp_p);



	/* ------------------------- Queue specific ------------------------------ */


	/**
	* @brief Initialize queue
	* @param  pointer to threadpool
	* @return 0 on success,
	*        -1 on memory allocation error
	*/
	static int thpool_jobqueue_init(thpool_t* tp_p);


	/**
	* @brief Add job to queue
	* 
	* A new job will be added to the queue. The new job MUST be allocated
	* before passed to this function or else other functions like thpool_jobqueue_empty()
	* will be broken.
	* 
	* @param pointer to threadpool
	* @param pointer to the new job(MUST BE ALLOCATED)
	* @return nothing 
	*/
	static void thpool_jobqueue_add(thpool_t* tp_p, thpool_job_t* newjob_p);


	/**
	* @brief Remove last job from queue. 
	* 
	* This does not free allocated memory so be sure to have peeked() \n
	* before invoking this as else there will result lost memory pointers.
	* 
	* @param  pointer to threadpool
	* @return 0 on success,
	*         -1 if queue is empty
	*/
	static int thpool_jobqueue_removelast(thpool_t* tp_p);


	/** 
	* @brief Get last job in queue (tail)
	* 
	* Gets the last job that is inside the queue. This will work even if the queue
	* is empty.
	* 
	* @param  pointer to threadpool structure
	* @return job a pointer to the last job in queue,
	*         a pointer to NULL if the queue is empty
	*/
	static thpool_job_t* thpool_jobqueue_peek(thpool_t* tp_p);


	/**
	* @brief Remove and deallocate all jobs in queue
	* 
	* This function will deallocate all jobs in the queue and set the
	* jobqueue to its initialization values, thus tail and head pointing
	* to NULL and amount of jobs equal to 0.
	* 
	* @param pointer to threadpool structure
	* */
	static void thpool_jobqueue_empty(thpool_t* tp_p);

private:

	static int thpool_keepalive;

	static pthread_mutex_t mutex;
};





#endif