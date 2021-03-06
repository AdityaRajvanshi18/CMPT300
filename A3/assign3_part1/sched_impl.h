#ifndef	__SCHED_IMPL__H__
#define	__SCHED_IMPL__H__
#include <semaphore.h>
#include "list.h"

struct thread_info {
	list_t* queue;
	list_elem_t* queueElement;
	sem_t sem;
};

struct sched_queue {
	list_elem_t* running;
	list_t* list;
};

#endif /* __SCHED_IMPL__H__ */
