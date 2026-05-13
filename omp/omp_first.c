#include <stdio.h>
#include <omp.h>
#include <unistd.h>

int main() {

	omp_set_num_threads(4);

	int x = 10;
	int y = 10;
	
	#pragma omp parallel firstprivate(x) shared(y)
	{

		#pragma omp master
		{
			printf("MASTER\n");
			sleep(5);

		}

		int tid = omp_get_thread_num();

		int nthreads = omp_get_num_threads();

		printf("x = %d\n", x);
		x++;
		
		#pragma omp barrier
		printf("AFTER BARRIER: %d\n", tid);
		
		
		// printf("%d/%d\n", tid, nthreads);
	}

	printf("x = %d\n", x);

	return 0;
}