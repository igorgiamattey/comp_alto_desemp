#pragma once
#include <cstdlib>

class Partition {
	int n; 			// total size of partition
	int n_procs; 	// number or processes
	int rank; 		// process id [0, n_procs - 1]

public:

	Partition(int n, int n_procs, int rank): n(n), n_procs(n_procs), rank(rank) { }

	// begin of a specific partition
	int begin() const {
		if (n % n_procs > rank ){
			int chunk = (n/n_procs) + 1;
			return rank * chunk;
		}

		else {
			int n_procs_unb = n % n_procs;
			int chunk_unb = (n/n_procs) + 1;
			int chunk = (n/n_procs);
			return (n_procs_unb * chunk_unb) + (rank - n_procs_unb) * chunk;
			
		}
	}

	int begin(int srank) const {
		if (n % n_procs > srank ){
			int chunk = (n/n_procs) + 1;
			return srank * chunk;
		}

		else {
			int n_procs_unb = n % n_procs;
			int chunk_unb = (n/n_procs) + 1;
			int chunk = (n/n_procs);
			return (n_procs_unb * chunk_unb) + (srank - n_procs_unb) * chunk;
			
		}
	}

	// end of a specific partition
	int end() const {
		return begin() + chunk() - 1;
	}

	int end(int srank) const {
		return begin(srank) + chunk(srank) - 1;
	}

	int chunk() const {
		if (n % n_procs > rank ){
			return (n/n_procs) + 1;
		}

		else {
			return (n/n_procs);
		}
	}

	int chunk(int srank) const {
		if (n % n_procs > srank ){
			return (n/n_procs) + 1;
		}

		else {
			return (n/n_procs);
		}
	}

	int size() const {
		return n;
	}

	int * chunks() const {

		int *c = (int *) malloc(sizeof(int) * n_procs);

		for (int i = 0; i < n_procs; i++){
			c[i] = chunk(i);
		}
		
		return c;
	}

	int * displ() const {

		int *d = (int *) malloc(sizeof(int) * n_procs);
		for (int i = 0; i < n_procs; i++){
			d[i] = begin(i);
		}
		return d;
	}
};