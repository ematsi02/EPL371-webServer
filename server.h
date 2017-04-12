#ifndef SERVER_H_
#define SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <limits.h>
#include <sys/stat.h>
#include <pthread.h>

#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

pthread_mutex_t request_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t got_request = PTHREAD_COND_INITIALIZER;
int num_of_requests = 0;

struct request {
   int id;
   int newsock;
   struct request* next;
};

struct request* head_request = NULL;
struct request* tail_request = NULL;

void add_request(int request_num, int newsock);
struct request* get_request(pthread_mutex_t* p_mutex);
void handle_request(struct request* a_request);
void* handle_requests_loop();
char* fileExtension(char* method);
int openCon(int port);

#endif
