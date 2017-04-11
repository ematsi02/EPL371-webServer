#include "queue.h"

Queue *ConstructQueue() {
    Queue *queue = (Queue*) malloc(sizeof (Queue));
    if (queue == NULL) {
        return NULL;
    }
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void DestructQueue(Queue *queue) {
    NODE *p;
    while (isEmpty(queue) != 0) {
        p = Dequeue(queue);
        free(p);
    }
    free(queue);
}

int Enqueue(Queue *pQueue, pthread_t tid) {
   NODE *p = NULL;
   p = (NODE *)malloc(sizeof(NODE));
   if (p == NULL) {
	perror("malloc");
	exit(1);
   }
   p->tid = tid;
   p->next = NULL;
   if (pQueue->size == 0)
     pQueue->head = pQueue->tail = p;
   else {
     pQueue->tail->next = p;
     pQueue->tail = p;
   }
   (pQueue->size)++;
   return 0;
}

int Dequeue(Queue *pQueue) {
   NODE *p = NULL;
   if ((pQueue == NULL) || (pQueue->head == NULL)) {
	perror("empty queue");
	exit(1);
   }
   p = pQueue->head;
   pQueue->head = pQueue->head->next;
   if (pQueue->head == NULL)
     	pQueue->tail = NULL;
   free(p);
   return 0;
}

int isEmpty(Queue* pQueue) {
    if (pQueue == NULL) {
        perror("empty queue");
	exit(1);
    }
    if (pQueue->size == 0) {
        return 0;
    } else {
        return -1;
    }
}
