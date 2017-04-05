#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int sock, newsock, serverlen, clientlen;
  char buf [256], *action;
  int port = 30000;
  struct sockaddr_in self, server, client;
  struct sockaddr *serverptr, *clientptr;
  struct hostent *rem;
  

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

  while(1) {
    clientptr = (struct sockaddr *) &client;
    clientlen = sizeof(client);

    if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
      perror("accept");
      exit(1);
    }
    
    if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
      herror("gethostbyaddr");
      exit(1);
    }
    printf("Accepted connection from %s\n", rem->h_name);
    
    bzero(buf, sizeof buf);
    if (read(newsock, buf, sizeof buf) < 0) {
      perror("read");
      exit(1);
    }
    printf("Read string: %s\n", buf);
    action = strtok(buf, " ");
    if (strcmp(action, "GET")) {
      action = strtok(buf, " ");
    }
    else if (strcmp(action, "HEAD")) {
      action = strtok(buf, " ");
    }
    else if (strcmp(action, "DELETE")) {
      action = strtok(buf, " ");
    }
    else {

    }
  }
   close(newsock);
  close(sock);
  return 0;
}
