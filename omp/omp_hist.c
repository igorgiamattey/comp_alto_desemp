#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static inline double wtime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}

void hist (char * txt, int h[256], long size){
	for (long i = 0; i < 256; i++){
		h[i] = 0;
	}

	#pragma omp parallel for if(size > 10000)
	for (long i = 0; i < size; i++){
		#pragma omp atomic
		h[(unsigned char) txt[i]]++;
	}
}

void hist_v2 (char * txt, int h[256], long size){
	
	for (long i = 0; i < 256; i++){
		h[i] = 0;
	}

	#pragma omp parallel
	{
		char hist_local[256];
		for (long i = 0; i < 256; i++){
			hist_local[i] = 0;
		}

		#pragma omp for
		for (long i = 0; i < size; i++){
			hist_local[(unsigned char) txt[i]]++;
		}

		#pragma omp critical
		{
			for (long i = 0; i < 256; i++){
				h[i] += hist_local[i];
			}
		}
	}
	
}

int main(int argc, char ** argv) {
	long n = atol(argv[1]);
	char * txt = (char *) malloc(n);
	
	for (long i = 0; i < n; ++i){
		txt[i] = rand() % 256;
	}	
	
	int h[256];

	double start = wtime();
	hist_v2(txt, h, n);
	double end = wtime();

	printf("%lf seconds\n", end - start);

	free(txt);
	return 0;
}