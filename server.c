#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int sock, newsock, serverlen, clientlen;
  char buf [256], *action;
  int port = 30000;
  struct sockaddr_in self, server, client;
  struct sockaddr *serverptr, *clientptr;
  struct hostent *rem;
  char *answerCode="HTTP/1.1 ";
  char *serverName="\r\nServer: ";
  char *contentLen="\r\nContent-Length: ";
  char *connection="\r\nConnection: ";
  char *contentType="\r\nContent-Type: ";
  //char* buf501="HTTP/1.1 501 Not Implemented\r\nServer:%s"+"\r\nContent-Length:%d"+"\r\nConnection:%s"+"\r\nContent-Type:%s"+"\r\n\r\n";
  char* buf501="HTTP/1.1 501 Not Implemented\r\nServer: myServer\r\nContent-Length: 24\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\nMethod not implemented!\n";
  char* content501="\r\n\r\nMethod not implemented!\n";
  

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
      // send(newsock,strcat(answerCode,"Not Implemented"),strlen(answerCode)+15,0);
	    write(newsock,buf501,strlen(buf501));
    }
  }
   close(newsock);
  close(sock);
  return 0;
}
