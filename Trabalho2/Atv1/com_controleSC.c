/*
Nomes:
	1) Piero A. L. Ribeiro; RA: 148052
	2) Vinicius L. Mello; RA: 140470
	3) Paulo R. C. Pedro; RA: 147906
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#define NUM_WORKERS 4

int request = -1, respond = 0, SOMA=0, next=0;

void server(){
	printf("SERVER - iniciado\n");
	while(1){
		while(request==-1){}
		//printf("	SERVER - request recebido\n");
		respond = request;
		while(respond !=-1){}
		request = -1;
		printf("	SERVER - soma: %d\n", SOMA);
		respond = rand()%NUM_WORKERS;
	}
}

void* client(void* args){
	int i = *((int*)args);
	next = 0;
	//printf("CLIENT %d - iniciado\n", i);
	// non-critical section
	while(1){
		while(respond!=i){}
		//printf("\nCLIENT %d - fazendo request\n", i);
		request = i;
		// critical section
		int local = SOMA;
		usleep((rand()%2)*1000);
		SOMA = local + 1;
		printf("	CLIENT %d - mandando pra resposta\n", i);
		respond = -1;
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
