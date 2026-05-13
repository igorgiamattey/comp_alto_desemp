#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv){
	MPI_Init(&argc, &argv);
	
	int rank, size;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int send[4];

	for (int i = 0; i < 4; i++) {
		send[i] = rank * 4 + i;
	}

	int recv[4];

	MPI_Alltoall(send, 1, MPI_INT, recv, 1, MPI_INT, MPI_COMM_WORLD);

	for (int i = 0; i < 4; i++) {
		printf("[%d] %d\n", rank, recv[i]);
	}

	MPI_Finalize();

	return 0;
}