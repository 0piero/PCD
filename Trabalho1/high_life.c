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
#include <wchar.h>
#include <locale.h>

pthread_barrier_t barrier;

int NUM_GEN = 10;
int GRID_SIZE = 36;
int NUM_WORKERS = 8;
int vivos = 0;

typedef struct {
    int shift;
    int** grid_ptr;
    int** newgrid_ptr;
    int i;
} thread_args;

/*
Admita que a posição (0,0) identifica a célula no canto superior esquerdo
1) Qualquer célula viva com 2 (dois) ou 3 (três) vizinhos deve sobreviver;
2) Qualquer célula morta com 3 (três) ou 6 (seis) vizinhos torna-se viva;
3) Qualquer outro caso, células vivas devem morrer e células já mortas devem continuar mortas.
*/


// mod -1 nao funciona, falei, C eh C

int getNeighbors(int** grid, int i, int j){ // -> quantidade de vizinhos vivos para a entrada a_{ij}
	int i_low = (i+1)%GRID_SIZE, i_high = (i-1+GRID_SIZE)%GRID_SIZE, j_low = (j-1+GRID_SIZE)%GRID_SIZE, j_high = (j+1)%GRID_SIZE;
	
	int pos[8][2] = {{i_low, j_low}, {i_low, j}, {i_low, j_high},
	{i, j_low}, {i, j_high},
	{i_high, j_low}, {i_high, j}, {i_high, j_high}};
	int count=0;
	for(int c=0;c<8;c++){
		if(grid[pos[c][0]][pos[c][1]] == 1){
			//if(i==3 && j==2){wprintf(L"sum: %d %d\n", pos[c][0], pos[c][1]);}
			count++;
		}
	}
	if(i==3 && j==2){wprintf(L"count32: %d\n", count);}
	return count;
	//return grid[i_low][j_high] + grid[i_low][j_low] + grid[i_low][j] +
	//	grid[i_high][j_high] + grid[i_high][j_low] + grid[i_high][j] + 
	//	grid[i][j_high] + grid[i][j_low];
}

void update_grid(int*** grid_ptr, int*** newgrid_ptr){
	int** aux = *grid_ptr;
	*grid_ptr = *newgrid_ptr;
	*newgrid_ptr = aux;
}
/*
void update_grid(int*** grid_ptr, int*** newgrid_ptr){
	for(int i=0;i<GRID_SIZE;i++){
		for(int j=0;j<GRID_SIZE;j++){
			*(grid_ptr)[i][j] = newgrid_ptr[i][j];
		}
	}
}
*/

int getAlive(int** grid, int shift)/* -> quantidade viva total 
										shift: posição que cada worker começa a busca sequencial
										(e.g. {shift, shift+NUM_WORKERS, ... , shift%NUM_WORKERS + NUM_WORKERS*(floor(GRID_SIZE^2/NUM_WORKERS) - 1)})										
								   */{
	int q = 0, **ptr = grid, *ptr2 = NULL;

	for(;ptr<grid+GRID_SIZE;ptr++){
        for(ptr2=*ptr+shift;ptr2<*ptr+GRID_SIZE;ptr2+=NUM_WORKERS){
        	if(*ptr2==1){q++;}
			//printf("%d\n", *ptr2);
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
			//printf("%c", grid_ptr[i][j]);
		}
		wprintf(L"\n");
	}
}

void print_2grids(int** grid_ptr, int** grid_ptr_new){
	wprintf(L"  ");
	for(int i=0;i<GRID_SIZE;i++){
		wprintf(L"%d ", i);
	}
	wprintf(L"   ");
	for(int i=0;i<GRID_SIZE;i++){
		wprintf(L"%d ", i);
	}
	wprintf(L"\n");
	for(int i=0;i<GRID_SIZE;i++){
		wprintf(L"%d ", i);
		for(int j=0;j<GRID_SIZE;j++){
			if(grid_ptr[i][j]==1){wprintf(L"%lc ", 0x25A0);}
			else{wprintf(L"%lc ", 0x25A1);}
			//printf("%c", grid_ptr[i][j]);
		}
		wprintf(L"   ", 0x25A1);
		for(int j=0;j<GRID_SIZE;j++){
			if(grid_ptr_new[i][j]==1){wprintf(L"%lc ", 0x25A0);}
			else{wprintf(L"%lc ", 0x25A1);}
			//printf("%c", grid_ptr[i][j]);
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
				//wprintf(L"jk: %d %d\n", j, k);
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
		
		
		pthread_barrier_wait(&barrier);
		// thread helper atualiza os grids
		// pthread_barrier_wait(&barrier);
		// recomeca com os grids atualizados
		if(arg->i == 0){
			print_2grids(arg->grid_ptr, arg->newgrid_ptr);
		}
		pthread_barrier_wait(&barrier);
		int** aux = arg->grid_ptr;
		arg->grid_ptr = arg->newgrid_ptr;
		arg->newgrid_ptr = aux;
		pthread_barrier_wait(&barrier);
		usleep(80000);
	}
}

void* thread_helper(void* arg1){
	thread_args* arg = (thread_args*) arg1;
	for(int i=0;i<NUM_GEN;i++){
		pthread_barrier_wait(&barrier);
		/*wprintf(L"\nGEN %d\n", i);
		wprintf(L"grid pre\n");
		print_grid(arg->grid_ptr);
		wprintf(L"new grid pre\n");
		print_grid(arg->newgrid_ptr);*/
		wprintf(L"PRE\n");
		print_2grids(arg->grid_ptr, arg->newgrid_ptr);
		update_grid(&(arg->grid_ptr), &(arg->newgrid_ptr));
		/*wprintf(L"grid pos\n");
		print_grid(arg->grid_ptr);
		wprintf(L"new grid pos\n");
		print_grid(arg->newgrid_ptr);
		wprintf(L"\n");*/
		wprintf(L"POS\n");
		print_2grids(arg->grid_ptr, arg->newgrid_ptr);
		pthread_barrier_wait(&barrier);
	}
}

void init_args(thread_args* arg, int shift, int** grid_ptr, int** newgrid_ptr, int i){ 
	arg->shift = shift;
	arg->grid_ptr = grid_ptr;
	arg->newgrid_ptr = newgrid_ptr;
	arg->i = i;
}


int main(int argc, char** argv){
	int** grid = (int**) malloc(GRID_SIZE * sizeof(int*));
	int** newgrid = (int**) malloc(GRID_SIZE * sizeof(int*));
	int j, shift = 0, i;
	
	if(argc > 1){
		NUM_GEN = atoi(argv[1]);
	}

	setlocale(LC_CTYPE, "");
	
	for(i=0;i<GRID_SIZE;i++){
		grid[i] = (int*) calloc(GRID_SIZE , sizeof(int));
		newgrid[i] = (int*) calloc(GRID_SIZE , sizeof(int));
	}

	pthread_t tid[NUM_WORKERS+1];
	pthread_barrier_init (&barrier, NULL, NUM_WORKERS);

	//GLIDER
	/*int lin = 1, col = 1;
	grid[lin  ][col+1] = 1;
	grid[lin+1][col+2] = 1;
	grid[lin+2][col  ] = 1;
	grid[lin+2][col+1] = 1;
	grid[lin+2][col+2] = 1;*/
	//R-pentomino
	int lin =10, col = 30;
	grid[lin  ][col+1] = 1;
	grid[lin  ][col+2] = 1;
	grid[lin+1][col  ] = 1;
	grid[lin+1][col+1] = 1;
	grid[lin+2][col+1] = 1;

	
	print_grid(grid);
	for(j=0;j<NUM_WORKERS;j++){
		thread_args* arg;
		arg = (thread_args*)malloc(sizeof(thread_args));
		init_args(arg, shift, grid, newgrid, j);
		pthread_create(&(tid[j]), NULL, runGeneration, (void*) arg);
		shift++;
    }
    //thread_args* arg;
	//arg = (thread_args*)malloc(sizeof(thread_args));
	//init_args(arg, shift, grid, newgrid);
    //pthread_create(&(tid[NUM_WORKERS]), NULL, thread_helper, (void*) arg);
    for(j=0; j<NUM_WORKERS; j++){
    	pthread_join(tid[j], NULL);
    }
    wprintf(L"vivos: %d\n", getAlive(grid, shift));
	print_grid(grid);


	return 0;
    
}

// TODO: o grid nao atualiza direito
