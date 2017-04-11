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

#define THREADS 40
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

static pthread_mutex_t mutex;
static pthread_cond_t cond;

char* fileExtension(char* method);
void accept_request(int newsock);
int openCon(int sock, struct sockaddr_in server, struct sockaddr *serverptr, int serverlen, int port);

#endif
