#include "server.h"

void add_request(int request_num, int newsock) {
	int err;
	struct request* a_request;

	a_request = (struct request*) malloc(sizeof(struct request));
	if (!a_request) {
		perror("malloc");
		exit(1);
	}
	a_request->id = request_num;
	a_request->newsock = newsock;
	a_request->next = NULL;

	if ((err = pthread_mutex_lock(&request_mutex)) != 0) {
		perror2("pthread_mutex_lock", err);
		exit(1);
	}

	if (num_of_requests == 0) {
		head_request = a_request;
		tail_request = a_request;
	} else {
		tail_request->next = a_request;
		tail_request = a_request;
	}

	num_of_requests++;

	if ((err = pthread_mutex_unlock(&request_mutex)) != 0) {
		perror2("pthread_mutex_unlock", err);
		exit(1);
	}

	if ((err = pthread_cond_signal(&got_request)) != 0) {
		perror2("pthread_mutex_lock", err);
		exit(1);
	}
}

struct request* get_request(pthread_mutex_t* p_mutex) {
	int err;
	struct request* a_request;

	if (num_of_requests > 0) {
		a_request = head_request;
		head_request = a_request->next;
		if (head_request == NULL) {
			tail_request = NULL;
		}

		num_of_requests--;
	} else {
		a_request = NULL;
	}

	if ((err = pthread_mutex_unlock(p_mutex)) != 0) {
		perror2("pthread_mutex_lock", err);
		exit(1);
	}

	return a_request;
}

void handle_request(struct request* a_request) {

	char* method = NULL;
	char buf[100000], response[100000], content[100000], requestmsg[100000];
	FILE *fp;
	char *contenttype;
	struct stat fstat;
	char *connection = NULL;

	do {
		bzero(response, sizeof(response));
		bzero(requestmsg, sizeof(requestmsg));
		bzero(buf, sizeof buf);
		if (read(a_request->newsock, buf, sizeof(buf)) < 0) {
			perror("read");
			exit(1);
		}
		strncpy(requestmsg, buf, strlen(buf));
		printf("%s\n", buf);

		if (strstr(requestmsg, "Connection: close") != NULL)
			connection = "close";
		else
			connection = "keep-alive";

		method = strtok(buf, " ");

		if (strcmp(method, "GET") == 0) {
			bzero(content, sizeof(content));

			method = strtok(NULL, " ");
			memmove(method, method + 1, strlen(method));

			contenttype = fileExtension(method);

			if (lstat(method, &fstat) < 0) {
				sprintf(response,
						"HTTP/1.1 404 Not Found\r\nServer: myServer\r\nContent-Length: 20\r\nConnection: %s\r\nContent-Type: text/plain\r\n\r\nDocument not found!\r\n",
						connection);
				printf("%s", response);
				write(a_request->newsock, response, strlen(response));
			} else {
				fp = fopen(method, "r");
				fread(content, sizeof(char), fstat.st_size + 1, fp);
				fclose(fp);

				sprintf(response,
						"HTTP/1.1 200 OK\r\nServer: myServer\r\nContent-Length: %d\r\nConnection: %s\r\nContent-Type: %s\r\n\r\n",
						fstat.st_size, connection, contenttype);

				printf("%s", response);
				write(a_request->newsock, response, strlen(response));
				write(a_request->newsock, content, fstat.st_size);
				write(a_request->newsock, "\r\n", 1);
			}
		} else if (strcmp(method, "HEAD") == 0) {
			method = strtok(NULL, " ");
			memmove(method, method + 1, strlen(method));

			contenttype = fileExtension(method);

			if (lstat(method, &fstat) < 0) {
				sprintf(response,
						"HTTP/1.1 404 Not Found\r\nServer: myServer\r\nContent-Length: 20\r\nConnection: %s\r\nContent-Type: text/plain\r\n\r\nDocument not found!\r\n",
						connection);
				printf("%s", response);
				write(a_request->newsock, response, strlen(response));
			} else {
				sprintf(response,
						"HTTP/1.1 200 OK\r\nServer: myServer\r\nContent-Length: %d\r\nConnection: %s\r\nContent-Type: %s\r\n\r\n",
						fstat.st_size, connection, contenttype);
				printf("%s", response);
				write(a_request->newsock, response, strlen(response));
				write(a_request->newsock, "\r\n", 1);
			}

		} else if (strcmp(method, "DELETE") == 0) {

			method = strtok(NULL, " ");
			memmove(method, method + 1, strlen(method));

			contenttype = fileExtension(method);

			if (lstat(method, &fstat) < 0) {
				sprintf(response,
						"HTTP/1.1 404 Not Found\r\nServer: myServer\r\nContent-Length: 20\r\nConnection: %s\r\nContent-Type: text/plain\r\n\r\nDocument not found!\r\n",
						connection);
				printf("%s", response);
				write(a_request->newsock, response, strlen(response));
			} else {
				if (remove(method) == -1) {
					perror("remove");
					exit(1);
				} else {
					sprintf(response,
							"HTTP/1.1 200 OK\r\nServer: myServer\r\nContent-Length: %d\r\nConnection: %s\r\nContent-Type: %s\r\n\r\n",
							fstat.st_size, connection, contenttype);
					write(a_request->newsock, response, strlen(response));
					write(a_request->newsock, "\r\n", 1);
				}
			}
		} else {
			sprintf(response,
					"HTTP/1.1 501 Not Implemented\r\nServer: myServer\r\nContent-Length: 24\r\nConnection: %s\r\nContent-Type: text/plain\r\n\r\nMethod not implemented!\r\n",
					connection);
			printf("%s", response);
			write(a_request->newsock, response, strlen(response));
		}
	} while (strstr(requestmsg, "Connection: close") == NULL);
	close(a_request->newsock);

}

void* handle_requests_loop() {
	int err;
	struct request* a_request;

	if ((err = pthread_mutex_lock(&request_mutex)) != 0) {
		perror2("pthread_mutex_lock", err);
		exit(1);
	}

	while (1) {
		if (num_of_requests > 0) {
			a_request = get_request(&request_mutex);
			if (a_request) {
				handle_request(a_request);
				free(a_request);
			}
		} else {
			if ((err = pthread_cond_wait(&got_request, &request_mutex)) != 0) {
				perror2("pthread_cond_wait", err);
				exit(1);
			}
		}
	}
}

char* fileExtension(char* method) {
	char* contenttype = NULL;

	if (strcmp(strrchr(method, '.'), ".txt") == 0
			|| strcmp(strrchr(method, '.'), ".sed") == 0
			|| strcmp(strrchr(method, '.'), ".awk") == 0
			|| strcmp(strrchr(method, '.'), ".c") == 0
			|| strcmp(strrchr(method, '.'), ".h") == 0
			|| strcmp(strrchr(method, '.'), ".java") == 0)
		contenttype = "text/plain";
	else if (strcmp(strrchr(method, '.'), ".html") == 0
			|| strcmp(strrchr(method, '.'), ".htm") == 0)
		contenttype = "text/html";
	else if (strcmp(strrchr(method, '.'), ".css") == 0)
		contenttype = "text/css";
	else if (strcmp(strrchr(method, '.'), ".jpeg") == 0
			|| strcmp(strrchr(method, '.'), ".jpg") == 0)
		contenttype = "image/jpeg";
	else if (strcmp(strrchr(method, '.'), ".gif") == 0)
		contenttype = "image/gif";
	else if (strcmp(strrchr(method, '.'), ".png") == 0)
		contenttype = "image/png";
	else if (strcmp(strrchr(method, '.'), ".pdf") == 0)
		contenttype = "application/pdf";
	else if (strcmp(strrchr(method, '.'), ".xml") == 0)
		contenttype = "application/xml";
	else if (strcmp(strrchr(method, '.'), ".zip") == 0)
		contenttype = "application/zip";
	else if (strcmp(strrchr(method, '.'), ".js") == 0)
		contenttype = "application/javascript";
	else
		contenttype = "application/octet-stream";

	return contenttype;
}

int openCon(int port) {
	int sock, serverlen;
	struct sockaddr_in server;
	struct sockaddr *serverptr;

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	serverptr = (struct sockaddr *) &server;
	serverlen = sizeof(server);

	if (bind(sock, serverptr, serverlen) < 0) {
		perror("bind");
		exit(1);
	}

	if (listen(sock, 5) < 0) {
		perror("listen");
		exit(1);
	}

	return sock;
}

int main(int argc, char *argv[]) {
	FILE *fp;
	char s[42];
	if ((fp = fopen("config.txt", "r")) == NULL) {
		perror("fopen");
		exit(1);
	}
	int sock, newsock, clientlen, port, threads, err, i;
	for (i=1; i<=7; i++) {
		if (i==4) 
			fscanf(fp, "THREADS=%d", &threads);
		else if (i==7)
			fscanf(fp, "PORT=%d", &port);
		else if (i==2 || i==5)
			fscanf(fp, "\n");
		else
			fscanf(fp, "%[^\n]\n", s);
	}
	struct sockaddr_in client;
	struct sockaddr *clientptr;
	struct hostent *rem;
	int thr_id[threads];
	pthread_t p_threads[threads];

	for (i = 0; i < threads; i++) {
		thr_id[i] = i;
		if ((err = pthread_create(&p_threads[i], NULL, handle_requests_loop,
		NULL)) != 0) {
			perror2("pthread_create", err);
			exit(1);
		}
	}

	sock = openCon(port);

	while (1) {

		clientptr = (struct sockaddr *) &client;
		clientlen = sizeof(client);

		if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
			perror("accept");
			exit(1);
		}

		if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,
				sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
			herror("gethostbyaddr");
			exit(1);
		}
		printf("Accepted connection from %s\n", rem->h_name);
		add_request(num_of_requests, newsock);

	}
	close(sock);
	return 0;
}
