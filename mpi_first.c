#include <mpi.h>
#include <stdio.h>

int main (int argc, char ** argv) {
	MPI_Init(&argc, &argv);

	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int N = 10000; // (2*5)**4
	int chunk = N/size;

	double s = 0;
	int init = chunk * rank;
	int final = init + chunk;

	for (int i = init; i < final; i++){
		double x = i * (1./N);
		s += 1./(1. + x*x);
	}

	MPI_Send(&s, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

	if (rank == 0){
		double s_final = 0;
		for (int i = 0; i < size; i++){
			double s_recv;
			MPI_Status status;
			MPI_Recv(&s_recv, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
			s_final += s_recv;
		}
		printf("pi = %lf\n", (4./N)*s_final);
	}
	
	MPI_Finalize();
	
	return 0;
}