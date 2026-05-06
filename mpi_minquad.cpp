#include <iostream>
#include <cmath>
#include <cstdlib>
#include <mpi.h>
#include <unistd.h>
#include "partition.h"

double avg (double *v_local, const Partition& p){
	double s = 0.0;

	for (int i = 0; i < p.chunk(); i++) {
		s += v_local[i];
	}
	
	double local_s = s/p.size();
	double global_s = 0.0;

	/*
	MPI_Allreduce(
		const void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, MPI_Comm comm
	);
	*/

	MPI_Allreduce(
		&local_s, &global_s, 1,
		MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD
	);

	return global_s;
}

double var (double *v_local, double avg_value, const Partition& p){
	double local_s = 0.0;

	for (int i = 0; i < p.chunk(); i++) {
		local_s += std::pow((v_local[i] - avg_value),2);
	}
	
	double global_s = 0.0;

	MPI_Reduce(
		&local_s, &global_s, 1,
		MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD
	);

	return global_s;
}

double cov (double *v_local, double v_avg, double *w_local, double w_avg, const Partition& p){
	double local_s = 0.0;

	for (int i = 0; i < p.chunk(); i++) {
		local_s += (v_local[i] - v_avg) * (w_local[i] - w_avg);
	}
	
	double global_s = 0.0;

	MPI_Reduce(
		&local_s, &global_s, 1,
		MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD
	);

	return global_s;
}

int main (int argc, char ** argv){
	MPI_Init(&argc, &argv);

	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int n = 8000;

	Partition p(n, size, rank);

	double *x, *y;
	
	if (rank == 0) {
		x = (double *) malloc(sizeof(double) * p.size());
		y = (double *) malloc(sizeof(double) * p.size());

		for (int i = 0; i < p.size(); i++) {
			x[i] = i;
			y[i] = 2 * i + 5;
		}
	}

	double *x_local = (double *) malloc(sizeof(double) * p.chunk());
	double *y_local = (double *) malloc(sizeof(double) * p.chunk());
	
	/* 
	MPI_Scatterv(
		const void *sendbuf, const int sendcounts[], const int displs[],
        MPI_Datatype sendtype, void *recvbuf, int recvcount,
        MPI_Datatype recvtype, int root, MPI_Comm comm
	);
	*/

	auto *ptr_chunks = p.chunks();
	auto *ptr_displ = p.displ();

	MPI_Scatterv(
		x, ptr_chunks, ptr_displ,
		MPI_DOUBLE, x_local, p.chunk(),
		MPI_DOUBLE, 0, MPI_COMM_WORLD
	);

	MPI_Scatterv(
		y, ptr_chunks, ptr_displ,
		MPI_DOUBLE, y_local, p.chunk(),
		MPI_DOUBLE, 0, MPI_COMM_WORLD
	);

	double x_avg = avg(x_local, p);
	double y_avg = avg(y_local, p);

	double cov_xy = cov(x_local, x_avg, y_local, y_avg, p);
	double var_x = var(x_local, x_avg, p);
	double var_y = var(y_local, y_avg, p);

	if (rank == 0) {
		double alpha, beta, rho;
		beta = cov_xy/var_x;
		alpha = y_avg - beta * x_avg;
		rho = cov_xy/std::sqrt(var_x * var_y);

		printf("Alpha:\t%f\nBeta:\t%f\nRho:\t%f\n", alpha, beta, rho);
		free(x);
		free(y);
	}

	free(x_local);
	free(y_local);
	free(ptr_chunks);
	free(ptr_displ);

	MPI_Finalize();

	return 0;
}