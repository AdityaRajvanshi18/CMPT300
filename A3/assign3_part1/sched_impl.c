#include "scheduler.h"
#include "sched_impl.h"
#include <stdlib.h>
#include <sched.h>

sem_t threadCon; 
sem_t mutex; 
sem_t threadEmp; 

/*** THREAD OPERATIONS ***/

static void init_thread_info(thread_info_t *info, sched_queue_t *queue){
	info->queue = queue->list;
	info->queueElement = NULL;
}


static void destroy_thread_info(thread_info_t *info){
	free(info->queueElement);
}


static void enter_sched_queue(thread_info_t *info){
	sem_wait(&threadCon);
	info->queueElement = (list_elem_t*)malloc(sizeof(list_elem_t));
	list_elem_init(info->queueElement, (void*)info);
	list_insert_tail(info->queue, info->queueElement);
	if(list_size(info->queue) == 1)
		sem_post(&threadEmp);
	sem_init(&info->sem,0,0);
}


static void leave_sched_queue(thread_info_t *info){
	list_remove_elem(info->queue, info->queueElement);
	sem_post(&threadCon);
}


static void wait_for_cpu(thread_info_t *info){
	sem_wait(&info->sem);
}


static void release_cpu(thread_info_t *info){
	sem_post(&mutex);
	sched_yield();
}

/*** SCHEDULER OPERATIONS ***/

static void init_sched_queue(sched_queue_t *queue, int queue_size){
	if (queue_size <= 0){
		exit(-1);
	}
	queue->running = NULL;
	queue->list = (list_t*) malloc(sizeof(list_t));
	list_init(queue->list);
	sem_init(&threadCon, 0, queue_size);
	sem_init(&mutex,0,0);
	sem_init(&threadEmp,0,0);
}

static void destroy_sched_queue(sched_queue_t *queue){
	list_elem_t * temp;
	while ((temp = list_get_head(queue->list)) != NULL) {
		list_remove_elem(queue->list, temp);
		free(temp);
	}
	free(queue->list);
}

static void wake_up_worker(thread_info_t *info){
	sem_post(&info->sem);
}

static void wait_for_worker(sched_queue_t *queue){
	sem_wait(&mutex);
}

static thread_info_t * rr_next_worker(sched_queue_t *queue){
	if(list_size(queue->list) == 0){
		return NULL;
	}
	int size = 0;
	if((queue->running != NULL) && ((size = list_size(queue->list)) > 1)) {
		list_remove_elem(queue->list, queue->running);
		if (size > list_size(queue->list)) {
			list_insert_tail(queue->list, queue->running);
		}
	}
	queue->running = list_get_head(queue->list);
	return (thread_info_t*)queue->running->datum;
}

static thread_info_t * fifo_next_worker(sched_queue_t *queue) {
	if(list_size(queue->list) == 0) {
		return NULL;
	}
	else {
		return (thread_info_t*) (list_get_head(queue->list))->datum;
	}
}

static void wait_for_queue(sched_queue_t *queue)
{
	sem_wait(&threadEmp);
}

sched_impl_t sched_fifo = {
	{ 
		init_thread_info, 
		destroy_thread_info, 
		enter_sched_queue, 
		leave_sched_queue, 
		wait_for_cpu, 
		release_cpu
	}, 
	{ 
		init_sched_queue, 
		destroy_sched_queue, 
		wake_up_worker, 
		wait_for_worker, 
		fifo_next_worker, 
		wait_for_queue
	} 
};
sched_impl_t sched_rr = {
	{ 
		init_thread_info, 
		destroy_thread_info, 
		enter_sched_queue, 
		leave_sched_queue, 
		wait_for_cpu, 
		release_cpu
	}, 
	{ 
		init_sched_queue, 
		destroy_sched_queue, 
		wake_up_worker, 
		wait_for_worker, 
		rr_next_worker, 
		wait_for_queue
	} 
};
