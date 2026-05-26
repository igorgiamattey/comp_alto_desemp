#include <iostream>
#include <cmath>
#include <cstdlib>
#include <mpi.h>
#include <unistd.h>
#include <string>

#define CHUNK 10000
#define TAG_REQ 1
#define TAG_RES 2
#define TAG_WRK 3

int isPrime (long x){
	int divisors = 0;

	for (int i = 2; i < x; ++i)
		if (x % i == 0)
			++divisors;

	if (divisors == 0)
		return 1;

	return 0;

}

void master (int size, long N){
	long next_n = 2;
	int active_workers = size - 1;
	bool finished = false;
	char buffer;

	while (active_workers >= 1) {
		// MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
		MPI_Status status;
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		if (status.MPI_TAG == TAG_REQ) {
			MPI_Request request, request_recv;
			long send_buf = finished ? -1 : next_n;

			MPI_Irecv(
				&buffer, 1, MPI_CHAR,
				status.MPI_SOURCE, TAG_REQ, MPI_COMM_WORLD, &request_recv
			);
			
			MPI_Isend (
				&send_buf, 1, MPI_LONG, status.MPI_SOURCE,
				TAG_WRK, MPI_COMM_WORLD, &request
			);
			
			if (!finished) {
				next_n += CHUNK;

				if (next_n > N) {
					finished = true;
				}
			}

			else {
				active_workers--;
			}
		}

		else if (status.MPI_TAG == TAG_RES) {
			long primes[CHUNK];
			int count;
			MPI_Get_count(&status, MPI_LONG, &count);
			MPI_Recv(
				primes, count, MPI_LONG,
				status.MPI_SOURCE, TAG_RES, MPI_COMM_WORLD, MPI_STATUS_IGNORE
			);

			for (int i = 0; i < count; i++)
				printf("%ld\n", primes[i]);
		}
	}
}


void worker (int N) {
	long next_n = 0;
	char buff;
	
	while (next_n != -1) {

		MPI_Send(
			&buff, 1, MPI_CHAR, 0,
			TAG_REQ, MPI_COMM_WORLD
		);

		MPI_Status status;

		MPI_Recv(
			&next_n, 1, MPI_LONG,
			0, TAG_WRK, MPI_COMM_WORLD, &status
		);

		if (next_n == -1) break;

		long primes[CHUNK];
		int k = 0;
		for (long p = next_n; p < next_n + CHUNK && p <= N; p++) {
			if (isPrime(p)) {
				primes[k] = p;
				k++;
			}
		}

		MPI_Send(
			primes, k, MPI_LONG, 0,
			TAG_RES, MPI_COMM_WORLD
		);
	}

}

int main (int argc, char ** argv){
	MPI_Init(&argc, &argv);

	int rank, size, n;

	n = std::stoi(argv[1]);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		master(size, n);
	}

	else {
		worker(n);
	}

	MPI_Finalize();

	return 0;
}