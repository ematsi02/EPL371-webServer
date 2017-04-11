#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

typedef struct Node {
    pthread_t tid;
    struct Node *next;
} NODE;

typedef struct Queue {
    NODE *head;
    NODE *tail;
    int size;
} Queue;

Queue *ConstructQueue();
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, pthread_t tid);
int Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);

#endif
