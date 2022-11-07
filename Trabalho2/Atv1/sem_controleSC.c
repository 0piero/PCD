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
#define NUM_WORKERS 2

int request = 0, respond = 0, SOMA=0;
pthread_barrier_t barrier;
pthread_barrier_t server_barrier;

void server(){
	printf("SERVER - iniciado\n");
	while(1){
		while(request==0){}
		respond = request;
		while(respond!=0){}
		request = 0;
		printf("	SERVER: %d\n", SOMA);
		pthread_barrier_wait(&server_barrier);
	}
}

void* client(void* args){
	int i = *((int*)args);
	while(1){

		request = i;
		while(respond==0){;;;;;;;;}
		/* ---------------------------------- */
		int local = SOMA;
		printf("	CLIENT %d {SOMA ANTES: %d}\n", i, local);
		
		SOMA = local + 1;
		printf("	CLIENT %d {SOMA DEPOIS: %d}\n", i, SOMA);
		/* ---------------------------------- */
		pthread_barrier_wait(&barrier);

		respond = 0;
		
		pthread_barrier_wait(&server_barrier);
	}
}

int main(){
	pthread_t pid[NUM_WORKERS];
	pthread_barrier_init(&barrier, NULL, NUM_WORKERS);
	pthread_barrier_init(&server_barrier, NULL, NUM_WORKERS+1);
	for(int i=1; i<NUM_WORKERS+1; i++){
		int* arg = (int*) malloc(sizeof(int));
		*arg = i;
		pthread_create(&(pid[i]), NULL, client, (void*) arg);
	}

	server();

	return 0;
}
