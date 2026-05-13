#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv){
	MPI_Init(&argc, &argv);

	int rank; int size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0){
		printf("RANK: %d\n", rank);
		sleep(4);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	printf("[%d] After barrier\n", rank);	
	
	MPI_Finalize();
	return 0;

}