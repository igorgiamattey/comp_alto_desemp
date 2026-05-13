#include <iostream>
#include <cmath>
#include <cstdlib>
#include <mpi.h>
#include <unistd.h>
#include "partition.h"

// r = v + w
void vec_sum(double *r, double *v, double *w, const Partition& p){
	for (int i = 0; i < p.chunk(); i++){
		r[i] = v[i] + w[i];
	}
}


int main (int argc, char ** argv){
	MPI_Init(&argc, &argv);

	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int n = 8;
	
	double *r, *v, *w;
	double *r_local, *v_local, *w_local;
	
	Partition p(n, size, rank);

	r_local = (double *) malloc(sizeof(double) * p.chunk());
	v_local = (double *) malloc(sizeof(double) * p.chunk());
	w_local = (double *) malloc(sizeof(double) * p.chunk());

	if (rank == 0) {
		r = (double *) malloc(sizeof(double) * p.size());
		v = (double *) malloc(sizeof(double) * p.size());
		w = (double *) malloc(sizeof(double) * p.size());

		for (int i = 0; i < p.size(); i++) {
			v[i] = w[i] = i;
		}
	}

	/* 
	MPI_Scatterv(
		const void *sendbuf, const int sendcounts[], const int displs[],
        MPI_Datatype sendtype, void *recvbuf, int recvcount,
        MPI_Datatype recvtype, int root, MPI_Comm comm
	);
	*/


	MPI_Scatterv(
		v, p.chunks(), p.displ(),
		MPI_DOUBLE, v_local, p.chunk(),
		MPI_DOUBLE, 0, MPI_COMM_WORLD
	);

	MPI_Scatterv(
		w, p.chunks(), p.displ(),
		MPI_DOUBLE, w_local, p.chunk(),
		MPI_DOUBLE, 0, MPI_COMM_WORLD
	);

	vec_sum(r_local, v_local, w_local, p);

	/*
	MPI_Gatherv(
		const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, const int recvcounts[], const int displs[],
		MPI_Datatype recvtype, int root, MPI_Comm comm
	);
	*/
	
	MPI_Gatherv( r_local, p.chunk(), MPI_DOUBLE,
		r, p.chunks(), p.displ(),
		MPI_DOUBLE, 0, MPI_COMM_WORLD
	);

	if (rank == 0) {		
		for (int i = 0; i < p.size(); i++) {
			printf("%f\n", r[i]);
		}	
	}

	MPI_Finalize();

	return 0;
}

