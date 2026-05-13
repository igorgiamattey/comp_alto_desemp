#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

int main(){

	long n = 1000000000;
	int k = 16;
	int * v = (int *) malloc(4*n);

	#pragma omp parallel for ordered(1) num_threads(4)
	for (int i = k; i < n; ++i){

		#pragma omp depend(sink: i-k)
		v[i] += v[i-k];
		#pragma omp depend(source: i)
	}
}