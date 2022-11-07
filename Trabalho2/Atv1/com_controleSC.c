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

void server(){
	printf("SERVER - iniciado\n");
	while(1){
		while(request==0){} // 					(5)

		respond = request; // 					(6)
	
		while(respond !=0){} //					(7)
		request = 0; // 						(8)
		printf("	SERVER: %d\n", SOMA);
	}
}

void* client(void* args){
	int i = *((int*)args);

	while(1){
		// non-critical section
		while(respond!=i){//					(1)
			request = i;//						(2)
		}
		
		// critical section //					(3)
		int local = SOMA;
		usleep(rand()%100);
		SOMA = local + 1;
		//                 //
		
		printf("	CLIENT %d\n", i);
		respond = 0; //							(4) 	
	}
}

int main(){
	pthread_t pid[NUM_WORKERS];
	for(int i=1; i<NUM_WORKERS+1; i++){
		int* arg = (int*) malloc(sizeof(int));
		*arg = i;
		pthread_create(&(pid[i]), NULL, client, (void*) arg);
	}

	server();

	return 0;
}
