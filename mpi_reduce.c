#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv){
	MPI_Init(&argc, &argv);

	int send_buf[] = {1, 2};
	int rank;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int recv_buf[] = {0, 0};


	MPI_Reduce(&send_buf, &recv_buf, 2, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0){
		printf("recv = %d %d\n", recv_buf[0], recv_buf[1]);
	}

	MPI_Finalize();
	return 0;

}