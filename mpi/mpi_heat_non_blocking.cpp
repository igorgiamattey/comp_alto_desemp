#include <iostream>
#include <cmath>
#include <cstdlib>
#include <mpi.h>
#include <unistd.h>
#include <string>
#include "partition.h"

bool isBorder (int i, int j, int n) {
	return (i == 0 || i == n-1 || j == 0 || j == n-1);
}

void printM (double **T,  int n, int r, const Partition& p) {
	for(int i = 1; i < p.chunk()+1; i++) {
		for(int j = 0; j < n; j++) {
			printf("[%d] %.3f ", r, T[i][j]);
		}
		printf("\n");
	}
}

void initMat (double **&T, int n, int s) {
	T = (double **) malloc(sizeof(double *) * (s));
	for (int i = 0; i < s; i++) {
		T[i] = (double *) malloc(sizeof(double) * n);
	}
}

void fillMat (double **T, int n) {
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			if (i == 0 && j == 1) {
				T[i][j] = 100;
			}
			else {
				T[i][j] = 0;
			}
		}
	}
}

void sendChunks (double **T, int displ, int chunk, int n, int destination) {
	for (int i = 0; i < chunk; i++) {
		MPI_Request request_send;
		MPI_Isend(T[displ + i], n, MPI_DOUBLE, destination, 0, MPI_COMM_WORLD, &request_send);
	}	
}

void copyChunkTo0 (double **T_src, double **T_dest, int chunk, int n) {
	for (int i = 0; i < chunk; i++) {
		for (int j = 0; j < n; j++) {
			T_dest[i+1][j] = T_src[i][j];
		}
	}
}

void addGhostRows (int rank, int size, double **T, const Partition& p, int n) {
	int up = rank - 1;
	int down = rank + 1;

	if (up < 0) up = MPI_PROC_NULL;
	if (down > size - 1) down = MPI_PROC_NULL;

	/*
	MPI_Sendrecv(
		const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		int dest, int sendtag, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, int source, int recvtag,
		MPI_Comm comm, MPI_Status *status
	);
	*/
	
	MPI_Sendrecv(
		T[p.chunk()], n, MPI_DOUBLE,
		down, 0, T[p.chunk() + 1], n,
		MPI_DOUBLE, down, 0,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE	
	);

	MPI_Sendrecv(
		T[1], n, MPI_DOUBLE,
		up, 0, T[0], n,
		MPI_DOUBLE, up, 0,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE	
	);
}

void calculateNextMat (const Partition& p, int n, double **T_next, double **T_prev) {
	for (int i = 1; i < p.chunk()+1; i++) {
		for (int j = 1; j < n-1; j++) {
			int global_i = p.getGlobalIdx(i - 1);

			if (!isBorder(global_i, j, n)){
				T_next[i][j] = 0.25 * (
					T_prev[i][j + 1] + 
					T_prev[i][j - 1] + 
					T_prev[i + 1][j] + 
					T_prev[i - 1][j]
				);
			}
		}
	}
}

void updatePrevMat (const Partition& p, int n, double **T_next, double **T_prev) {
	for (int i = 1; i < p.chunk()+1; i++) {
		for (int j = 1; j < n-1; j++) {
			int global_i = p.getGlobalIdx(i - 1);
			if (!isBorder(global_i, j, n))
				T_prev[i][j] = T_next[i][j];
		}
	}
}

void freeMat (double **T, int rows) {
	for (int i = 0; i < rows; i++)
		free(T[i]);
	free(T);
}

int main (int argc, char ** argv){
	MPI_Init(&argc, &argv);

	int rank, size;
	int n_iter = 1, n = 12;

	if (argc > 1)
    	n_iter = std::stoi(argv[1]);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	Partition p(n, size, rank);

	double **T, **T_prev, **T_next;

	initMat(T_prev, n, p.chunk() + 2);
	initMat(T_next, n, p.chunk() + 2);
	
	if (rank == 0) {
		initMat(T, n, n);
		fillMat(T, n);
	}

	if (rank == 0) {
		auto *ptr_chunks = p.chunks();
		auto *ptr_displ = p.displ();

		for (int i = 0; i < size; i++)
			sendChunks(T, ptr_displ[i], ptr_chunks[i], n, i);
		
	}

	MPI_Request request[p.chunk()];

	for (int i = 0; i < p.chunk(); i++) {
		MPI_Irecv(T_prev[i + 1], n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &request[i]);
	}

	for (int i = 0; i < p.chunk(); i++) {
		MPI_Wait(&request[i], NULL);
	}
	
	for (int count = 0; count < n_iter; count++) {
		
		addGhostRows(rank, size, T_prev, p, n);

		calculateNextMat(p, n, T_next, T_prev);

		updatePrevMat(p, n, T_next, T_prev);
		
	}

	for (int r = 0; r < size; r++){ 
		MPI_Barrier(MPI_COMM_WORLD);

		if (r == rank) {
			printM(T_prev, n, r, p);
			fflush(stdout);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	freeMat(T_prev, p.chunk() + 2);
	freeMat(T_next, p.chunk() + 2);

	if (rank == 0)
		freeMat(T, n);

	MPI_Finalize();

	return 0;
}