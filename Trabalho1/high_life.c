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
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>
pthread_barrier_t barrier;

int NUM_GEN = 10;
int GRID_SIZE = 16;
int NUM_WORKERS = 8;

typedef struct {
    int shift;
    int** grid_ptr;
    int** newgrid_ptr;
} thread_args;

/*
Admita que a posição (0,0) identifica a célula no canto superior esquerdo
do tabuleiro e que a posição (N-1,N-1) identifica a célula no canto inferior direito. 
1) Qualquer célula viva com 2 (dois) ou 3 (três) vizinhos deve sobreviver;
2) Qualquer célula morta com 3 (três) ou 6 (seis) vizinhos torna-se viva;
3) Qualquer outro caso, células vivas devem morrer e células já mortas devem continuar mortas.
*/

int getNeighbors(int** grid, int i, int j){ 
	int i_low = (i+1)%GRID_SIZE, i_high = (i-1+GRID_SIZE)%GRID_SIZE, j_low = (j-1+GRID_SIZE)%GRID_SIZE, j_high = (j+1)%GRID_SIZE;
	
	int pos[8][2] = {{i_low, j_low}, {i_low, j}, {i_low, j_high},
	{i, j_low}, {i, j_high},
	{i_high, j_low}, {i_high, j}, {i_high, j_high}};
	int count=0;
	for(int c=0;c<8;c++){
		if(grid[pos[c][0]][pos[c][1]] == 1){
			count++;
		}
	}
	return count;
}

int getAlive(int** grid, int shift){
	int q = 0, **ptr = grid, *ptr2 = NULL;

	for(;ptr<grid+GRID_SIZE;ptr++){
        for(ptr2=*ptr+shift;ptr2<*ptr+GRID_SIZE;ptr2+=NUM_WORKERS){
        	if(*ptr2==1){q++;}
        }
        shift = ptr2-(*ptr+GRID_SIZE);
        if(shift<0){
			shift = 0;
        }
    }
    
    return q;
}

void print_grid(int** grid_ptr){
	for(int i=0;i<GRID_SIZE;i++){
		for(int j=0;j<GRID_SIZE;j++){
			if(grid_ptr[i][j]==1){wprintf(L"%lc ", 0x25A0);}
			else{wprintf(L"%lc ", 0x25A1);}
		}
		wprintf(L"\n");
	}
}

void print_2grids(int** grid_ptr, int** grid_ptr_new){
	wprintf(L"    ");
	for(int i=0;i<GRID_SIZE;i++){
		wprintf(L"%d ", i%10);
	}
	wprintf(L"   ");
	for(int i=0;i<GRID_SIZE;i++){
		wprintf(L"%d ", i%10);
	}
	wprintf(L"\n");
	for(int i=0;i<GRID_SIZE;i++){
		wprintf(L"%.3d ", i);
		for(int j=0;j<GRID_SIZE;j++){
			if(grid_ptr[i][j]==1){wprintf(L"%lc ", 0x25A0);}
			else{wprintf(L"%lc ", 0x25A1);}
		}
		wprintf(L"   ", 0x25A1);
		for(int j=0;j<GRID_SIZE;j++){
			if(grid_ptr_new[i][j]==1){wprintf(L"%lc ", 0x25A0);}
			else{wprintf(L"%lc ", 0x25A1);}
		}
		wprintf(L"\n");
	}
}

void* runGeneration(void* arg1){
	thread_args* arg = (thread_args*) arg1;

	for(int i=0;i<NUM_GEN;i++){
		int j = (arg->shift)/GRID_SIZE, k = (arg->shift)%GRID_SIZE;
		for(;j<GRID_SIZE;k=k%GRID_SIZE){
			for(;k<GRID_SIZE;k+=NUM_WORKERS, j+=k/GRID_SIZE){
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
					if(nn==3 || nn==6){
						(arg->newgrid_ptr)[j][k]=1;
					}
					else{
						(arg->newgrid_ptr)[j][k]=0;
					}
				}
			}
		}
		if(arg->shift == 0){
			print_2grids(arg->grid_ptr, arg->newgrid_ptr);
		}
		pthread_barrier_wait(&barrier);
		int** aux = arg->grid_ptr;
		arg->grid_ptr = arg->newgrid_ptr;
		arg->newgrid_ptr = aux;
		pthread_barrier_wait(&barrier);
	}
	int alive_count = getAlive(arg->grid_ptr, arg->shift);
	int* ret = (int*) malloc(sizeof(int));
	*ret = alive_count;
	pthread_exit((void*) ret);
}

void init_args(thread_args* arg, int shift, int** grid_ptr, int** newgrid_ptr){ 
	arg->shift = shift;
	arg->grid_ptr = grid_ptr;
	arg->newgrid_ptr = newgrid_ptr;
}


int main(int argc, char** argv){
	struct timeval inicio, final2;
	struct timeval inicio_concorrente, final2_concorrente;
	int tmili, tmili_concorrente;

	gettimeofday(&inicio, NULL);

	if(argc > 1){
		NUM_GEN = atoi(argv[1]);
	}if(argc > 2){
		NUM_WORKERS = atoi(argv[2]);
	}if(argc > 3){
		GRID_SIZE = atoi(argv[3]);
	}

	int** grid = (int**) malloc(GRID_SIZE * sizeof(int*));
	int** newgrid = (int**) malloc(GRID_SIZE * sizeof(int*));
	
	int j, shift = 0, i;
	time_t seconds;
     
    seconds = time(NULL);

	setlocale(LC_CTYPE, "");
	
	for(i=0;i<GRID_SIZE;i++){
		grid[i] = (int*) calloc(GRID_SIZE , sizeof(int));
		newgrid[i] = (int*) calloc(GRID_SIZE , sizeof(int));
	}

	pthread_t tid[NUM_WORKERS+1];
	pthread_barrier_init (&barrier, NULL, NUM_WORKERS);

	//GLIDER
	int lin = 1, col = 1;
	grid[lin  ][col+1] = 1;
	grid[lin+1][col+2] = 1;
	grid[lin+2][col  ] = 1;
	grid[lin+2][col+1] = 1;
	grid[lin+2][col+2] = 1;
	//R-pentomino
	/*int lin =10, col = 30;
	grid[lin  ][col+1] = 1;
	grid[lin  ][col+2] = 1;
	grid[lin+1][col  ] = 1;
	grid[lin+1][col+1] = 1;
	grid[lin+2][col+1] = 1;*/

	gettimeofday(&inicio_concorrente, NULL);
	for(j=0;j<NUM_WORKERS;j++){
		thread_args* arg;
		arg = (thread_args*)malloc(sizeof(thread_args));
		init_args(arg, shift, grid, newgrid);
		pthread_create(&(tid[j]), NULL, runGeneration, (void*) arg);
		shift++;
    }

	int* thread_count[NUM_WORKERS];
	int soma_total=0;
    for(j=0; j<NUM_WORKERS; j++){
    	pthread_join(tid[j], (void**) &(thread_count[j]));
    }
    gettimeofday(&final2_concorrente, NULL);
    for(int i=0;i<NUM_WORKERS;i++){
    	soma_total+=*(thread_count[i]);
    }

    wprintf(L"vivos: %d\n", soma_total);
    gettimeofday(&final2, NULL);

	tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);
	tmili_concorrente = (int) (1000 * (final2_concorrente.tv_sec - inicio_concorrente.tv_sec) + (final2_concorrente.tv_usec - inicio_concorrente.tv_usec) / 1000);  

	wprintf(L"tempo decorrido: %d milisegundos\n", tmili);
	wprintf(L"tempo trecho concorrente: %d milisegundos\n", tmili_concorrente);

	return 0;
    
}