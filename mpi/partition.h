#pragma once
#include <cstdlib>

class Partition {
	private:
		int n; 			// total size of partition
		int n_procs; 	// number or processes
		int rank; 		// process id [0, n_procs - 1]

	public:
		Partition(int n, int n_procs, int rank): n(n), n_procs(n_procs), rank(rank) { }

		// begin of the partition for a specific process
		int begin() const {		// current process
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

		int begin(int srank) const {	// process with rank srank
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

		// end of the partition for a specific process
		int end() const {	// current process
			return begin() + chunk() - 1;
		}

		int end(int srank) const {	// process with rank srank
			return begin(srank) + chunk(srank) - 1;
		}

		// get the size of the partition for a specific process
		int chunk() const {		// current process
			if (n % n_procs > rank ){
				return (n/n_procs) + 1;
			}

			else {
				return (n/n_procs);
			}
		}

		int chunk(int srank) const {	// process with rank srank
			if (n % n_procs > srank ){
				return (n/n_procs) + 1;
			}

			else {
				return (n/n_procs);
			}
		}
		
		// get the total size of the partition
		int size() const {
			return n;
		}

		// get the size of the partition for all processes (for Gatherv and Scatterv)
		int * chunks() const {
			int *c = (int *) malloc(sizeof(int) * n_procs);

			for (int i = 0; i < n_procs; i++){
				c[i] = chunk(i);
			}
			
			return c;
		}

		// get the starting displacement for all processes (for Gatherv and Scatterv)
		int * displ() const {
			int *d = (int *) malloc(sizeof(int) * n_procs);
			
			for (int i = 0; i < n_procs; i++){
				d[i] = begin(i);
			}
			
			return d;
		}

		int getGlobalIdx(int localIdx) const {
			return begin(rank) + localIdx;
		}
};

class Partition2D {
	private:
		// Matrix: 9 cols, 12 rows
		// 3 processes in columns, 2 in rows
		// nx = 9	ny = 12
		// n_procs_x = 3	n_procs_y = 2

		int nx;				// size of partition in x direction
		int ny; 			// size of partition in y direction
		int n_procs_x;		// number or processes in x direction
		int n_procs_y;		// number or processes in y direction
		int n_procs; 		// number or processes (n_procs = n_procs_x * n_procs_y)
		int rank; 			// process id [0, n_procs - 1]

		// TODO: Implement
};