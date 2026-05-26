#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main (int argc, char ** argv) {

	MPI_Init(&argc, &argv);

	int rank, size;
	char name[32];
	int result_length;

	MPI_Get_processor_name(name, &result_length);
	printf("name = %s\n", name);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 1) {
		MPI_Request request;
		sleep(10);
		
		/* 
		MPI_Isend(
			const void *buf, int count, MPI_Datatype datatype, int dest,
			int tag, MPI_Comm comm, MPI_Request *request
		);
		*/
		
		MPI_Isend(
			&rank, 1, MPI_INT, 0,
			0, MPI_COMM_WORLD, &request
		);
	}

	else if (rank == 0) {
		MPI_Request request;

		int buffer;
		printf("Before RECV\n");

		/* 
		MPI_Irecv(
			void *buf, int count, MPI_Datatype datatype,
			int source, int tag, MPI_Comm comm, MPI_Request *request
		);
		*/

		MPI_Irecv(
			&buffer, 1, MPI_INT,
			1, 0, MPI_COMM_WORLD, &request
		);

		printf("After RECV\n");
		MPI_Wait(&request, NULL);

		int flag = 0;
		double t0 = MPI_Wtime();
		double timeout = 5;
		int flag_timeout = 0;

		while (1) {
			MPI_Test(&request, &flag, NULL);

			if (flag != 0)
				break;
			
			double t1 = MPI_Wtime();
			if (t1 - t0 > timeout) {
				printf("TIMEOUT\n");
				flag_timeout = 1;
				break;
			}
			usleep(10);
			printf("FLAG = %d\n", flag);
		}
	}

	MPI_Finalize();
	return 0;
}