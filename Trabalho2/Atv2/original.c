/*
Nomes:
	1) Piero A. L. Ribeiro; RA: 148052
	2) Vinicius L. Mello; RA: 140470
	3) Paulo R. C. Pedro; RA: 147906
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <wchar.h>
#include <locale.h>
#include <sys/time.h>
int NUM_GEN = 2000;
int GRID_SIZE = 2048;
int NUM_WORKERS = 8;
int q = 0;

typedef struct {
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

int getNeighbors(int** grid, int i, int j){
	int i_low = (i+1)%GRID_SIZE, i_high = (i-1+GRID_SIZE)%GRID_SIZE, j_low = (j-1+GRID_SIZE)%GRID_SIZE, j_high = (j+1)%GRID_SIZE;
	int pos[8][2] = {{i_low, j_low}, {i_low, j}, {i_low, j_high},
	{i, j_low}, {i, j_high},
	{i_high, j_low}, {i_high, j}, {i_high, j_high}};
	int count=0;

	for(int c=0;c<8;c++){
		//printf("%d\n",grid[pos[c][0]][pos[c][1]]);
		if(grid[pos[c][0]][pos[c][1]] == 1){
			count++;
		}
	}
	return count;
}

int getAlive(int** grid){
	int i, j;

	#pragma omp for reduction(+:q)
		for(i=0; i<GRID_SIZE; i++){
    	    for(j=0; j<GRID_SIZE; j++){
    	    	if(grid[i][j]==1){
    	    		q++;
    	    	}
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

void print_grid_clipped(int** grid_ptr, int tam){
	if(tam>GRID_SIZE)
		tam = GRID_SIZE;
	wprintf(L"   ");
	for(int i=0;i<tam;i++){
		wprintf(L"%d ", i%10);
	}
	for(int i=0;i<tam;i++){
		wprintf(L"\n%.2d ", i);
		for(int j=0;j<tam;j++){
			if(grid_ptr[i][j]==1){wprintf(L"%lc ", 0x25A0);}
			else{wprintf(L"%lc ", 0x25A1);}
		}
	}

	wprintf(L"\n\n");
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
			//printf("%c", grid_ptr[i][j]);
		}
		wprintf(L"   ", 0x25A1);
		for(int j=0;j<GRID_SIZE;j++){
			if(grid_ptr_new[i][j]==1){wprintf(L"%lc ", 0x25A0);}
			else{wprintf(L"%lc ", 0x25A1);}
		}
		wprintf(L"\n");
	}
}

int simple_count(int** grid_ptr){
	int sum=0;
	for(int i=0;i<GRID_SIZE;i++){
		for(int j=0;j<GRID_SIZE;j++){
			sum += grid_ptr[i][j];
		}
	}
	return sum;
}


int runGeneration(void* arg1){
	thread_args arg = *((thread_args*) arg1);
	int i, j, k, alive_count=0;
	#pragma omp parallel num_threads(NUM_WORKERS) private(i, j, k) reduction(+: alive_count)
	{
		// segfault aq
		//printf("test\n");
		//wprintf(L"%d\n", (arg.grid_ptr)[0][0]);
		//printf("test\n");
		for(i=0;i<NUM_GEN;i++){
			#pragma omp for
				for(j=0;j<GRID_SIZE; j++){
					for(k=0;k<GRID_SIZE;k++){
						int nn = getNeighbors(arg.grid_ptr, j, k);
						//printf("Cheguei aqui agora");
						if((arg.grid_ptr)[j][k]==1){
							if(nn==2 || nn==3){
								(arg.newgrid_ptr)[j][k]=1;
							}
							else{
								(arg.newgrid_ptr)[j][k]=0;
							}
						}
						else{
							if(nn==3){
								(arg.newgrid_ptr)[j][k]=1;
							}
							else{
								(arg.newgrid_ptr)[j][k]=0;
							}
						}
						//printf("j = %d, k = %d\n",j,k);
					}
					//printf("Finalizado\n");
				}
			#pragma omp barrier 
			#pragma omp single
			{
				if(i<5){
					if(i>0){
						wprintf(L"Geração %d: %d\n", i, simple_count(arg.grid_ptr));
					}
					print_grid_clipped(arg.grid_ptr, 50);

				}
			}
			#pragma omp barrier 
			#pragma omp single
				{
			int** aux = arg.grid_ptr;
			arg.grid_ptr = arg.newgrid_ptr;
			arg.newgrid_ptr = aux;
				}
			#pragma omp barrier 
		}
		alive_count += getAlive(arg.grid_ptr);
	}
	//int* ret = (int*) malloc(sizeof(int));
	//*ret = alive_count;
	return alive_count;
}

void init_args(thread_args* arg, int** grid_ptr, int** newgrid_ptr){ 
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

	setlocale(LC_CTYPE, "");
	
	for(i=0;i<GRID_SIZE;i++){
		grid[i] = (int*) calloc(GRID_SIZE , sizeof(int));
		newgrid[i] = (int*) calloc(GRID_SIZE , sizeof(int));
	}

	//GLIDER
	int lin = 1, col = 1;
	grid[lin  ][col+1] = 1;
	grid[lin+1][col+2] = 1;
	grid[lin+2][col  ] = 1;
	grid[lin+2][col+1] = 1;
	grid[lin+2][col+2] = 1;
	//R-pentomino
	lin =10, col = 30;
	grid[lin  ][col+1] = 1;
	grid[lin  ][col+2] = 1;
	grid[lin+1][col  ] = 1;
	grid[lin+1][col+1] = 1;
	grid[lin+2][col+1] = 1;

	wprintf(L"** Game of Life OMP\n");
	wprintf(L"\nCondição inicial: %d\n", simple_count(grid));

	thread_args* arg;
	arg = (thread_args*)malloc(sizeof(thread_args));
	init_args(arg, grid, newgrid);
	runGeneration((void*) arg);
	q=0;
	gettimeofday(&inicio_concorrente, NULL);
	int soma_total = getAlive(grid);
	gettimeofday(&final2_concorrente, NULL);

    wprintf(L"\n...\n");
    wprintf(L"Última geração (%d iterações): %d células vivas\n", NUM_GEN, soma_total);
	gettimeofday(&final2, NULL);
	
	tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);
	tmili_concorrente = (int) (1000000 * (final2_concorrente.tv_sec - inicio_concorrente.tv_sec) + (final2_concorrente.tv_usec - inicio_concorrente.tv_usec));  

	wprintf(L"tempo decorrido: %d milisegundos\n", tmili);
	wprintf(L"tempo trecho da última contagem: %d microssegundos\n", tmili_concorrente);
	
	return 0;
}
