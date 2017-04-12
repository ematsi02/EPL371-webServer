#include "server.c"

void testExtension(){
	char* type1=fileExtension("index.html");
	char* type2=fileExtension("file.txt");
	char* type3=fileExtension("image.png");
	char* type4=fileExtension("folder.zip");
}

void testRequestQueue() {
	add_request(1, 11);
	add_request(2, 12);
	add_request(3, 13);
	add_request(4, 14);

	struct request *p = head_request;
	while (p->next != NULL) {
		printf("id= %d ", p->id);
		printf("socket= %d\n", p->newsock);
		p = p->next;
	}
}

int main(int argc, char *argv[]) {

	testExtension();
	testRequestQueue();

	return 0;
}


