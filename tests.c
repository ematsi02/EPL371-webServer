#ifdef DEBUG
#include "server.c"

void testExtension(){
	char *type = NULL;
	printf("DEBUG for function fileExtension :\n\n");
	type = fileExtension("test.txt");
	printf("File extension for test.txt is: %s\n", type);
	type = fileExtension("test.sed");
	printf("File extension for test.sed is: %s\n", type);
	type = fileExtension("test.awk");
	printf("File extension for test.awk is: %s\n", type);
	type = fileExtension("test.c");
	printf("File extension for test.c is: %s\n", type);
	type = fileExtension("test.h");
	printf("File extension for test.h is: %s\n", type);
	type = fileExtension("test.java");
	printf("File extension for test.java is: %s\n", type);
	type = fileExtension("test.html");
	printf("File extension for test.html is: %s\n", type);
	type = fileExtension("test.htm");
	printf("File extension for test.htm is: %s\n", type);
	type = fileExtension("test.css");
	printf("File extension for test.css is: %s\n", type);
	type = fileExtension("test.jpeg");
	printf("File extension for test.jpeg is: %s\n", type);
	type = fileExtension("test.jpg");
	printf("File extension for test.jpg is: %s\n", type);
	type = fileExtension("test.gif");
	printf("File extension for test.gif is: %s\n", type);
	type = fileExtension("test.png");
	printf("File extension for test.png is: %s\n", type);
	type = fileExtension("test.pdf");
	printf("File extension for test.pdf is: %s\n", type);
	type = fileExtension("test.xml");
	printf("File extension for test.xml is: %s\n", type);
	type = fileExtension("test.zip");
	printf("File extension for test.zip is: %s\n", type);
	type = fileExtension("test.js");
	printf("File extension for test.js is: %s\n", type);
	type = fileExtension("test.unknown");
	printf("File extension for test.unknown is: %s\n", type);
	printf("\n");
}

void testRequestQueue() {
	printf("DEBUG for function add_request :\n\n");
	add_request(1, 11);
	add_request(2, 12);
	add_request(3, 13);
	add_request(4, 14);
	printf("List of requests:\n");
	struct request *p = head_request;
	while (p!= NULL) {
		printf("id = %d ", p->id);
		printf("socket = %d\n", p->newsock);
		p = p->next;
	}
	printf("\n");
}

void testGetRequest() {
	printf("DEBUG for function get_request :\n\n");
	struct request* r = NULL;
	r = get_request(&request_mutex);
	while (r!=NULL) { 
		printf("Got request %d, with sock %d\n", r->id, r->newsock);
		r = head_request;
		printf("List of requests:\n");
		while (r!= NULL) {
			printf("id = %d ", r->id);
			printf("socket = %d\n", r->newsock);
			r = r->next;
		}
		r = get_request(&request_mutex);
	}
	printf("\n");
}

int main(int argc, char *argv[]) {

	testExtension();
	testRequestQueue();
	testGetRequest();

	return 0;
}

#endif


