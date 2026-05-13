#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv){
	int world_size, rank, size;
	int send_data[8], recv_data[2];
	
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0){
		for (int i = 0; i < 8; i++){
			send_data[i] = i;
		}
	}

	MPI_Scatter(send_data, 2, MPI_INT, recv_data, 2, MPI_INT, 0, MPI_COMM_WORLD);

	printf("Rank %d received: [%d, %d]\n", rank, recv_data[0], recv_data[1]);

	MPI_Finalize();

	return 0;
}

