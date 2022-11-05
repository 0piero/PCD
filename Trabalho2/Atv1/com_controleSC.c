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

int request = 0, respond = 0, SOMA=0, next=0;

void server(){
	printf("SERVER - iniciado\n");
	while(1){
		while(request==0){} // 					(6)

		respond = request; // 					(7)
		while(respond !=0){} //					(8)
		request = 0; // 						(9)
		printf("	SERVER: %d\n", SOMA);
		//respond = rand()%NUM_WORKERS;
	}
}

void* client(void* args){
	int i = *((int*)args);
	next = 0;
	//printf("CLIENT %d - iniciado\n", i);
	while(1){
		// non-critical section
		while(respond!=i){//					(1)
			request = i;//						(2)
		}
		
		// critical section //					(3)
		int local = SOMA;
		usleep(rand()%100);
		SOMA = local + 1;
		//
		
		printf("	CLIENT %d\n", i); // 		(4)
		respond = 0; //							(5) 	
	}
}

int main(){
	pthread_t pid[NUM_WORKERS];
	for(int i=1; i<NUM_WORKERS+1; i++){
		int arg = i;
		next = 1;
		pthread_create(&(pid[i]), NULL, client, (void*) &arg);
		while(next==1){}
	}

	server();

	return 0;
}
