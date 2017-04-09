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

int openCon(int sock, struct sockaddr_in self, struct sockaddr_in server,
		struct sockaddr *serverptr, int serverlen, int port) {
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

	if (bind(sock, serverptr, serverlen) < 0) { /* Bind socket to an address */
		perror("bind");
		exit(1);
	}
	/*Listen for connections*/
	if (listen(sock, 5) < 0) { /*5 max requests in queue*/
		perror("listen");
		exit(1);
	}

	return sock;

}

int main(int argc, char *argv[]) {
	int sock, newsock, serverlen, clientlen;
	int port = 8080;
	struct sockaddr_in self, server, client;
	struct sockaddr *serverptr, *clientptr;
	struct hostent *rem;
	char word[30];
	char* method = NULL;
	char buf[100000], response[100000], content[100000];
	char c;
	FILE *fp;
	int i;
	char *contenttype;
	struct stat fstat;

	/*for header*/
	char *answerCode = "HTTP/1.1 ";
	char *serverName = "\r\nServer: myServer ";
	char *contentLen = "\r\nContent-Length: ";
	char *connection = "\r\nConnection: ";
	char *contentType = "\r\nContent-Type: ";
	char* buf501 =
			"HTTP/1.1 501 Not Implemented\r\nServer: myServer\r\nContent-Length: 24\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\n\r\nMethod not implemented!\n";
	char* content501 = "\r\n\r\nMethod not implemented!\n";

	int sk = openCon(sock, self, server, serverptr, serverlen, port);

	while (1) {
		bzero(response, sizeof(response));
		clientptr = (struct sockaddr *) &client;
		clientlen = sizeof(client);

		/*Accept connection*/
		if ((newsock = accept(sk, clientptr, &clientlen)) < 0) {
			perror("accept");
			exit(1);
		}

		if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,
				sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
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

		method = strtok(buf, " ");

		if (strcmp(method, "GET") == 0) {
			bzero(content, sizeof(content));
			i = 0;

			method = strtok(NULL, " ");
			memmove(method, method + 1, strlen(method));

			if (lstat(method, &fstat) < 0) {
				perror("lstat error");
				exit(1);
			}

			if ((fp = fopen(method, "r")) == NULL) {
				sprintf(response,
						"HTTP/1.1 404 Not Found\r\nServer: localhost\r\nContent-Length: 20\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\n\r\nDocument not found!");
				write(newsock, response, strlen(response));
			} else {
				//while ((c=getc(fp)) != EOF)
				//  content[i++] = c;
				fread(content, sizeof(char), fstat.st_size + 1, fp);
				fclose(fp);
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

				sprintf(response,"HTTP/1.1 200 OK\r\nServer: localhost\r\nContent-Length: %zd\r\nConnection: keep-alive\r\nContent-Type: %s\r\n\r\n%s",
						fstat.st_size, contenttype, content);

				printf("%s", response);
				write(newsock, response, strlen(response));
			}
		} else if (strcmp(method, "HEAD") == 0) {
			printf("Read string: %s\n", buf);
			printf("mpika sto head");
		} else if (strcmp(method, "DELETE") == 0) {
			printf("Read string: %s\n", buf);
			printf("mpika sto delete");
		} else {
			printf("mpika sto else");

			sprintf(response,
					"%s501 Not Implemented%s%s%d%skeep-alive%stext/plain%s",
					answerCode, serverName, contentLen, strlen(content501),
					connection, contentType, content501);
			write(newsock, response, strlen(response));
//	   send(newsock,serverName,strlen(serverName),0);
//	   send(newsock,strcat(contentLen,(char*)strlen(content501)),strlen(contentLen)+2,0);
//	   send(newsock,strcat(connection,"keep-alive"),strlen(connection)+10,0);
//	   send(newsock,strcat(contentType,"text/plain"),strlen(contentType)+10,0);

			//write(newsock,buf501,strlen(buf501));

		}

		close(newsock);
	}
	close(sock);
	return 0;
}
