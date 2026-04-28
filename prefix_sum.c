#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

int main (int argc, char ** argv){
	
	int n = atoi(argv[1]);
	int * v = (int *) malloc(4*n);

	for (int i = 0; i < n; ++i) v[i] = 1;

	omp_set_num_threads(4);

	int nt = 4;

	printf("nt = %d\n", nt);

	int * prefix = (int *) malloc(4*nt);

	#pragma omp parallel num_threads(nt)
	{
		int chunk = n/nt;
		int tid = omp_get_thread_num();

		int init = tid * chunk + 1;
		int end = init + chunk - 2;

		for (int i = init; i <= end; ++i)
			v[i] += v[i-1];

		#pragma omp barrier

		#pragma omp single
		{
			prefix[1] = v[chunk - 1];
			for (int i = 2; i < nt; ++i){
				prefix[i] = prefix[i-1] + v[i * chunk - 1];
			}
		}

		#pragma omp barrier

		if (tid > 0){
			printf("[%d] %d\n", tid, prefix[tid]);
			for (int i = init; i <= end; ++i)
				v[i] += prefix[tid];
		}

	}

	for (int i = 0; i < n; ++i)
		printf("%d\n", v[i]);

	return 0;

}