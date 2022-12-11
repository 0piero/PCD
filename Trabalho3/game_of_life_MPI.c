/*
Nomes:
	1) Piero A. L. Ribeiro; RA: 148052
	2) Vinicius L. Mello; RA: 140470
	3) Paulo R. C. Pedro; RA: 147906
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <sys/time.h>
#include <mpi.h>

int NUM_GEN = 2000;
int GRID_SIZE = 2048;
int NUM_WORKERS = 1;

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
	int q = 0, i, j;

	//#pragma omp for private(i, j)	
	for(i=0; i<GRID_SIZE; i++){
    	for(j=0; j<GRID_SIZE; j++){
    	   	if(grid[i][j]==1){q++;}
    	}
    }
    return q;
}

int getAlive_sector(int** grid, int begin, int end){
	int q = 0, i, j;

	//#pragma omp for private(i, j)	
	for(i=begin; i<=end; i++){
    	for(j=0; j<GRID_SIZE; j++){
    	   	if(grid[i][j]==1){q++;}
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

int Begin_Function(int myrank, int nProc){
  if(myrank==0){
    return 0;
  }
  else{
    return (myrank) * GRID_SIZE/nProc + 1;
  }
}

int End_Function(int myrank, int nProc){
	if(myrank == (nProc - 1)){
		return GRID_SIZE-1;
	}
	else{
		return (myrank+1) * GRID_SIZE / nProc;
	}
}

int runGeneration(void* arg1){
  int ierr, myrank, nProc;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nProc);
	MPI_Status status[nProc];
  int sendArrayPreviousProc[GRID_SIZE], sendArrayNextProc[GRID_SIZE];
	int receiveArrayPreviousProc[GRID_SIZE], receiveArrayNextProc[GRID_SIZE];
	
	thread_args arg = *((thread_args*) arg1);
	
  int i, j, k, a, alive_count=0, receive_alive_count = 0;
	int begin, end;
  begin = Begin_Function(myrank, nProc);
  end = End_Function(myrank, nProc);
  for(i=0;i<NUM_GEN;i++){
		for(a = 0; a < GRID_SIZE;a++){
				(arg.grid_ptr)[(end+1)%GRID_SIZE][a] = receiveArrayNextProc[a];
				(arg.grid_ptr)[(begin-1+GRID_SIZE)%GRID_SIZE][a] = receiveArrayPreviousProc[a];
		}
		for(j=begin;j<=end; j++){
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
			int** aux = arg.grid_ptr;
			arg.grid_ptr = arg.newgrid_ptr;
			arg.newgrid_ptr = aux; 
			alive_count = getAlive_sector(arg.grid_ptr,begin, end);
      ierr = MPI_Barrier(MPI_COMM_WORLD);
			ierr = MPI_Reduce(&alive_count, &receive_alive_count, 1, MPI_INTEGER, MPI_SUM, 0, MPI_COMM_WORLD);
			for(a = 0; a < GRID_SIZE;a++){
				sendArrayNextProc[a] = (arg.grid_ptr)[end][a];
				sendArrayPreviousProc[a] = (arg.grid_ptr)[begin][a];
			}
			ierr = MPI_Sendrecv(&sendArrayPreviousProc, GRID_SIZE, MPI_INTEGER, ((myrank - 1+nProc)%nProc), 10, &receiveArrayPreviousProc, GRID_SIZE, MPI_INTEGER, ((myrank - 1+nProc)%nProc), 10, MPI_COMM_WORLD,status);
			ierr = MPI_Sendrecv(&sendArrayNextProc, GRID_SIZE, MPI_INTEGER, ((myrank + 1)%nProc), 20, &receiveArrayNextProc, GRID_SIZE, MPI_INTEGER, ((myrank + 1)%nProc), 20, MPI_COMM_WORLD, status);
			ierr = MPI_Barrier(MPI_COMM_WORLD);
			if(myrank == 0){
				printf("Geracao %d = %d\n", i, receive_alive_count);
			}
      ierr = MPI_Barrier(MPI_COMM_WORLD);
    }
    //printf("myrank = %d, nProc = %d begin = %d, end = %d\n",myrank,nProc, begin, end);
		alive_count += getAlive(arg.grid_ptr);
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
	
	/*if(argc > 1){
		NUM_GEN = atoi(argv[1]);
	}if(argc > 2){
		NUM_WORKERS = atoi(argv[2]);
	}if(argc > 3){
		GRID_SIZE = atoi(argv[3]);
	}*/

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

	int ierr;
	MPI_Init(&argc, &argv);

	thread_args* arg;
	arg = (thread_args*)malloc(sizeof(thread_args));
	init_args(arg, grid, newgrid);
	gettimeofday(&inicio_concorrente, NULL);
	int soma_total = runGeneration((void*) arg);
	
	ierr = MPI_Finalize();


	gettimeofday(&final2_concorrente, NULL);
	wprintf(L"vivos: %d\n", soma_total);
	gettimeofday(&final2, NULL);
	
	tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);
	tmili_concorrente = (int) (1000 * (final2_concorrente.tv_sec - inicio_concorrente.tv_sec) + (final2_concorrente.tv_usec - inicio_concorrente.tv_usec) / 1000);  

	wprintf(L"tempo decorrido: %d milisegundos\n", tmili);
	wprintf(L"tempo trecho concorrente: %d milisegundos\n", tmili_concorrente);
	
	return 0;
}
