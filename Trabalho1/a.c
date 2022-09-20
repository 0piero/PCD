/*
Nomes:
	1) Piero A. L. Ribeiro; RA: 148052
	2) Vinicius L. Mello; RA: 140470
	3)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int NUM_GEN = 5;
int GRID_SIZE = 5;
int NUM_WORKERS = 1;
int vivos = 0;

typedef struct {
    int* shift;
    int** grid_ptr;
    int** newgrid_ptr;
} thread_args;

/*
Admita que a posição (0,0) identifica a célula no canto superior esquerdo
do tabuleiro e que a posição (N-1,N-1) identifica a célula no canto inferior direito. 

1) Qualquer célula viva com 2 (dois) ou 3 (três) vizinhos deve sobreviver;
2) Qualquer célula morta com 3 (três) vizinhos torna-se viva;
3) Qualquer outro caso, células vivas devem morrer e células já mortas devem continuar mortas.
*/

int getNeighbors(int** grid, int i, int j)/* -> quantidade de vizinhos vivos para a entrada a_{ij}*/{ 
	int i_low = (i+1)%GRID_SIZE, i_high = (i-1)%GRID_SIZE, j_low = (j-1)%GRID_SIZE, j_high = (j+1)%GRID_SIZE;
	int pos[8][2] = {{i_low, j_high}, {i_low, j_low}, {i_low, j}, 
	{i_high, j_high}, {i_high, j_low}, {i_high, j},
	{i, j_high}, {i, j_low}};
	int count=0;
	for(int c=0;c<8;c++){
		if(grid[pos[c][0]][pos[c][1]] == 1){
			count++;
		}
	}
	return count;
}

int getAlive(int** grid, int shift)/* -> quantidade viva total 
										shift: posição que cada worker começa a busca sequencial
										(e.g. {shift, shift+NUM_WORKERS, ... , shift%NUM_WORKERS + NUM_WORKERS*(floor(GRID_SIZE^2/NUM_WORKERS) - 1)})										
								   */{
	int q = 0, **ptr = grid, *ptr2 = NULL;

	for(;ptr<grid+GRID_SIZE;ptr++){
        for(ptr2=*ptr+shift;ptr2<*ptr+GRID_SIZE;ptr2+=NUM_WORKERS){
        	if(*ptr2==1){q++;}
			printf("%d\n", *ptr2);
        }
        shift = ptr2-(*ptr+GRID_SIZE);
        if(shift<0){
			shift = 0;
        }
    }
    return q;
}

void* runGeneration(void* arg1){
	thread_args* arg = (thread_args*) arg1;
	int j = *(arg->shift)/GRID_SIZE, k = *(arg->shift)%GRID_SIZE;
	for(int i=0;i<NUM_GEN;i++){
		for(;j<GRID_SIZE;k=k%GRID_SIZE){
			for(;k<GRID_SIZE;k+=NUM_WORKERS, j+=k/GRID_SIZE){
				printf("%d %d\n", j, k);
				int nn = getNeighbors(arg->grid_ptr, j, k);
				if((arg->grid_ptr)[j][k]==1){
					if(nn==2 || nn==3){
						(arg->newgrid_ptr)[j][k]=1;
					}
					else{
						(arg->newgrid_ptr)[j][k]=0;
					}
				}
				else{
					if(nn==3){
						(arg->newgrid_ptr)[j][k]=1;
					}
					else{
						(arg->newgrid_ptr)[j][k]=0;
					}
				}
			}
		}
		int pthread_barrier_wait(pthread_barrier_t *barrier);
		//
	}
}

void update_grid(int** grid_ptr, int** newgrid_ptr){
	int** aux = grid_ptr;
	grid_ptr = newgrid_ptr;
	newgrid_ptr = aux;
}

void init_args(thread_args* arg, int shift, int** grid_ptr, int** newgrid_ptr){ 
	*(arg->shift) = shift;
	arg->grid_ptr = grid_ptr;
	arg->newgrid_ptr = newgrid_ptr;
}

int main(int argc, char** argv){
	int** grid = (int**) malloc(GRID_SIZE * sizeof(int*));
	int** newgrid = (int**) malloc(GRID_SIZE * sizeof(int*));
	int j, shift = 0, i;
	pthread_barrier_t bar;
	for(i=0;i<GRID_SIZE;i++){
		grid[i] = (int*) calloc(GRID_SIZE , sizeof(int));
		newgrid[i] = (int*) calloc(GRID_SIZE , sizeof(int));
	}

	pthread_t tid[NUM_WORKERS];
	//GLIDER
	int lin = 1, col = 1;
	grid[lin  ][col+1] = 1;
	grid[lin+1][col+2] = 1;
	grid[lin+2][col  ] = 1;
	grid[lin+2][col+1] = 1;
	grid[lin+2][col+2] = 1;
	
	
	for(j=0;j<NUM_WORKERS;j++){
		thread_args* arg;
		arg = (thread_args*)malloc(sizeof(thread_args));
		init_args(arg, shift, grid, newgrid);
		pthread_create(&(tid[j]), NULL, runGeneration, (void*) arg);
		shift++;
    }
    for(j=0; j<NUM_WORKERS; j++){
    	pthread_join(tid[j], NULL);
    }
    printf("vivos: %d\n", getAlive(grid, shift));
	

	//R-pentomino
	/*
	lin =10; col = 30;
	grid[lin  ][col+1] = 1;
	grid[lin  ][col+2] = 1;
	grid[lin+1][col  ] = 1;
	grid[lin+1][col+1] = 1;
	grid[lin+2][col+1] = 1;
	*/

	return 0;
    
}