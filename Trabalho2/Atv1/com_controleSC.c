#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define NUM_WORKERS 2

int request = -1, respond = 0, SOMA=0, next=0;

void server(){
	printf("SERVER - iniciado\n");
	while(1){
		while(request==-1){}
		//printf("	SERVER - request recebido\n");
		respond = request;
		while(respond!=0){}
		request = -1;
		printf("	SERVER - soma: %d\n", SOMA);
	}
}

void* client(void* args){
	int i =  *((int*)args);
	next = 0;
	//printf("CLIENT %d - iniciado\n", i);
	// non-critical section
	while(1){
		while(respond!=i){}
		//printf("\nCLIENT %d - fazendo request\n", i);
		request = i;
		// critical section
		int local = SOMA;
		sleep(rand()%2);
		SOMA = local + 1;
		//printf("	CLIENT %d - mandando pra resposta\n", i);
		respond = 0;
	}
}

int main(){
	pthread_t pid[NUM_WORKERS];
	for(int i=0; i<NUM_WORKERS; i++){
		int arg = i;
		next = 1;
		pthread_create(&(pid[i]), NULL, client, (void*) &arg);
		while(next==1){}
	}

	server();

	return 0;
}
