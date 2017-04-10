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

int openCon(int sock, struct sockaddr_in server, struct sockaddr *serverptr, int serverlen, int port) {
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

char* fileExtension(char* method){
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

int main(int argc, char *argv[]) {
  int sock, newsock, serverlen, clientlen;
  int port = 8080;
  struct sockaddr_in server, client;
  struct sockaddr *serverptr, *clientptr;
  struct hostent *rem;
  char word[30];
  char* method = NULL;
  char buf[100000], response[100000], content[100000], request[100000];
  char c;
  FILE *fp;
  char *contenttype;
  struct stat fstat;

  int sk = openCon(sock, server, serverptr, serverlen, port);

  while (1) {
    bzero(response, sizeof(response));
    clientptr = (struct sockaddr *) &client;
    clientlen = sizeof(client);

    if ((newsock = accept(sk, clientptr, &clientlen)) < 0) {
      perror("accept");
      exit(1);
    }

    if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
      herror("gethostbyaddr");
      exit(1);
    }
    printf("Accepted connection from %s\n", rem->h_name);

do {
    bzero(buf, sizeof buf); 
    if (read(newsock, buf, sizeof(buf)) < 0) { 
      perror("read");
      exit(1);
    } 
    strncpy(request, buf, strlen(buf));
    printf("Read string: %s\n", buf);

    method = strtok(buf, " ");

    if (strcmp(method, "GET") == 0) {
      bzero(content, sizeof(content));

      method = strtok(NULL, " ");
      memmove(method, method + 1, strlen(method));

      contenttype = fileExtension(method);

      if (lstat(method, &fstat) < 0) {
        sprintf(response, "HTTP/1.1 404 Not Found\r\nServer: localhost\r\nContent-Length: 20\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\n\r\nDocument not found!\r\n");
        printf("%s", response);
        write(newsock, response, strlen(response));
      }
      else {
        fp = fopen(method, "r");
        fread(content, sizeof(char), fstat.st_size + 1, fp);
       fclose(fp);


        sprintf(response,"HTTP/1.1 200 OK\r\nServer: localhost\r\nContent-Length: %d\r\nConnection: keep-alive\r\nContent-Type: %s\r\n\r\n",fstat.st_size, contenttype);

        printf("%s", response);
        write(newsock, response, strlen(response));
        write(newsock, content, fstat.st_size);
 	write(newsock, "\r\n", 1);
      }
    } else if (strcmp(method, "HEAD") == 0) {
      method = strtok(NULL, " ");
      memmove(method, method + 1, strlen(method));

      contenttype = fileExtension(method);

      if (lstat(method, &fstat) < 0) {
        sprintf(response, "HTTP/1.1 404 Not Found\r\nServer: localhost\r\nContent-Length: 20\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\n\r\nDocument not found!\r\n");
        printf("%s", response);
        write(newsock, response, strlen(response));
      }
      else {
       sprintf(response,"HTTP/1.1 200 OK\r\nServer: localhost\r\nContent-Length: %d\r\nConnection: keep-alive\r\nContent-Type: %s\r\n\r\n", fstat.st_size, contenttype);
       printf("%s", response);
       write(newsock, response, strlen(response));
       write(newsock, "\r\n", 1);
     }

   } else if (strcmp(method, "DELETE") == 0) {

			method = strtok(NULL, " ");
			memmove(method, method + 1, strlen(method));

contenttype = fileExtension(method);

      if (lstat(method, &fstat) < 0) {
        sprintf(response, "HTTP/1.1 404 Not Found\r\nServer: localhost\r\nContent-Length: 20\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\n\r\nDocument not found!\r\n");
        printf("%s", response);
        write(newsock, response, strlen(response));
      }
      else {
			if (remove(method) == -1) {
				perror("remove");
				exit(1);
			} else {
				sprintf(response,"HTTP/1.1 200 OK\r\nServer: localhost\r\nContent-Length: %d\r\nConnection: keep-alive\r\nContent-Type: %s\r\n\r\n", fstat.st_size, contenttype);
				write(newsock, response, strlen(response));
			        write(newsock, "\r\n", 1);
			}
}
		} else {
   sprintf(response, "HTTP/1.1 501 Not Implemented\r\nServer: localhost\r\nContent-Length: 24\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\n\r\nMethod not implemented!\r\n");
   printf("%s", response); 
   write(newsock, response, strlen(response));
 }
} while (strstr(request, "Connection: close")==NULL);
 close(newsock);
}
close(sock);
return 0;
}
