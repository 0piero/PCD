#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define NUM_WORKERS 2

int request = 0, respond = 0, SOMA=0;

void server(){
	while(1){
		while(request==0){}
		respond = request;
		while(respond!=0){}
		request = 0;
		printf("soma: %d", SOMA);
	}
}

void* client(void* args){
	int i =  *((int*)args);
	// non-critical section
	while(1){
		while(respond!=i){}
		request = i;
		// critical section
		int local = SOMA;
		sleep(rand()%2);
		SOMA = local + 1;
		respond = 0;
	}
}

int main(){
	pthread_t pid[NUM_WORKERS];
	for(int i=0; i<NUM_WORKERS; i++){
		int arg = i;
		pthread_create(&(pid[i]), NULL, client, (void*) &arg);
	}

	server();

	return 0;
}
