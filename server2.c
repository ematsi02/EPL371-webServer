#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  int sock, newsock, serverlen, clientlen;
  char buf [1000000], *action;
  int port = 30000;
  struct sockaddr_in server, client;
  struct sockaddr *serverptr, *clientptr;
  struct hostent *rem;
  char response[1000000];
  char content[1000000];
  char c;
  FILE *fp;
  int i, binary;
  char *contenttype;
  struct stat fstat;
  
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

  while(1) {
    bzero(response, sizeof(response));
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
    bzero(buf, sizeof(buf));
    if (read(newsock, buf, sizeof(buf)) < 0) {
      perror("read");
      exit(1);
    }
    if (buf==NULL) 
	    continue;
	printf("Read string: %s\n", buf);
    action = strtok(buf, " ");
    
    if (strcmp(action, "GET") == 0) {
      bzero(content, sizeof(content));
      i = 0;
      
	  action = strtok(NULL, " ");
	  memmove(action, action+1, strlen(action));
	    if (strcmp(strrchr(action, '.'), ".txt") == 0 || strcmp(strrchr(action, '.'), ".sed") == 0 || strcmp(strrchr(action, '.'), ".awk") == 0 || strcmp(strrchr(action, '.'), ".c") == 0 || strcmp(strrchr(action, '.'), ".h") == 0 || strcmp(strrchr(action, '.'), ".java") == 0) {
	      contenttype = "text/plain";
	      binary = 0;
	    }
	    else if (strcmp(strrchr(action, '.'), ".html") == 0 || strcmp(strrchr(action, '.'), ".htm") == 0) {
	      contenttype = "text/html";
	      binary = 0; 
	    }
	    else if (strcmp(strrchr(action, '.'), ".css") == 0) {
	      contenttype = "text/css";
	      binary = 0;
	    }
	    else if (strcmp(strrchr(action, '.'), ".jpeg") == 0 || strcmp(strrchr(action, '.'), ".jpg") == 0) {
	      contenttype = "image/jpeg";
	      binary = 1;
	    }
	    else if (strcmp(strrchr(action, '.'), ".gif") == 0) {
	      contenttype = "image/gif";
	      binary = 1;
	    }
	    else if (strcmp(strrchr(action, '.'), ".png") == 0) {
	      contenttype = "image/png";
	      binary = 1;
	    }
	    else if (strcmp(strrchr(action, '.'), ".pdf") == 0) {
	      contenttype = "application/pdf";
	      binary = 1;
	    }
	    else if (strcmp(strrchr(action, '.'), ".xml") == 0) {
	      contenttype = "application/xml";  
	      binary = 1;
	    }
	    else if (strcmp(strrchr(action, '.'), ".zip") == 0) {
	      contenttype = "application/zip";
	      binary = 1;
	    }
	    else if (strcmp(strrchr(action, '.'), ".js") == 0) {
	      contenttype = "application/javascript"; 
	      binary = 1;
	    }
	    else {
	      contenttype = "application/octet-stream";
	      binary = 1;
	    }
	    
	    if (lstat(action, &fstat) < 0) {
          sprintf(response, "HTTP/1.1 404 Not Found\r\nServer: localhost\r\nContent-Length: 20\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\n\r\nDocument not found!");
	      write(newsock, response, strlen(response));
        } 
        else {
	
	    if (binary==0) {
	      fp=fopen(action, "r");
	      fread(content, sizeof(char), fstat.st_size+1, fp);
	      sprintf(response, "HTTP/1.1 200 OK\r\nServer: localhost\r\nContent-Length: %d\r\nConnection: keep-alive\r\nContent-Type: %s\r\n\r\n", fstat.st_size, contenttype);
	    }
	      else {
	      fp=fopen(action, "rb");
	      fread(content, sizeof(char), fstat.st_size+1, fp);
	      sprintf(response, "HTTP/1.1 200 OK\r\nServer: localhost\r\nContent-Length: %d\r\nConnection: keep-alive\r\nContent-Type: %s\r\n\r\n", fstat.st_size, contenttype);
	   }
	    fclose(fp);
	     printf("%s", response); 
     	write(newsock, response, strlen(response));
     	write(newsock, content, fstat.st_size);
      }
    }
    else if (strcmp(action, "HEAD") == 0) {
      printf("Read string: %s\n", buf);
    }
    else if (strcmp(action, "DELETE") == 0) {
      printf("Read string: %s\n", buf);
    }
    else {
	  sprintf(response, "HTTP/1.1 501 Not Implemented\r\nServer: localhost\r\nContent-Length: 24\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\nMethod not implemented!");
	   printf("%s", response); 
	  write(newsock, response, strlen(response));
    }
     close(newsock);
  }
  close(sock);
  return 0;
}
