#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int main(int argc, char *argv[]) {
  int sock, newsock, serverlen, clientlen;
  int port = 8080;
  char buf[256];
  struct sockaddr_in self, server, client;
  struct sockaddr *serverptr, *clientptr;
  struct hostent *rem;
  

  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket");
    exit(1);
  }

  bzero(&self, sizeof(self));
  server.sin_family = AF_INET; /*Internet domain*/
  server.sin_addr.s_addr = htonl(INADDR_ANY); /*My Internet address*/
  server.sin_port = htons(port); 
  serverptr = (struct sockaddr *) &server;
  serverlen = sizeof(server);

  /*Bind socket to address*/
  if (bind(sock, (struct sockaddr*)&self, sizeof(self)) < 0) {
    perror("bind");
    exit(1);
  }

  /*Listen for connections*/
  if (listen(sock, 5) < 0) { /*5 max requests in queue*/
    perror("listen");
    exit(1);
  }

  while(1) {
    clientptr = (struct sockaddr *) &client;
    clientlen = sizeof(client);

    /*Accept connection*/
    if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
      perror("accept");
      exit(1);
    }
    
    if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
      herror("gethostbyaddr");
      exit(1);
    }
    printf("Accepted connection from %s\n", rem->h_name);
    
    bzero(buf, sizeof buf); /* Initialize buffer */
    if (read(newsock, buf, sizeof buf) < 0) { /* Receive message */
        perror("read");
        exit(1);
     }
     printf("Read string: %s\n", buf);
    close(newsock);
  }
  close(sock);
  return 0;
}
