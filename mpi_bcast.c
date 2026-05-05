#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv){
	int N = 10;
	int size, rank;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int * v_local = (int *) malloc(N * sizeof(int));

	if (rank == 0){
		for (int i = 0; i < N; i++){
			v_local[i] = i;
		}
	}

	MPI_Bcast(v_local, N, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 5){
		for (int i = 0; i < N; i++){
			printf("[%d] %d\n", i, v_local[i]);
		}
	}

	MPI_Finalize();

	return 0;
}