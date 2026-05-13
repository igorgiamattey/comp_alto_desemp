#include <stdio.h>
#include <omp.h>
#include <unistd.h>

int main() {

	omp_set_num_threads(4);

	int N = 1000;
	int *x = malloc(N*4);
	int *y = malloc(N*4);
	int alpha = -3;

	#pragma omp parallel for
	for (int i = 0; i < N; ++i){
		x[i] = y[i] = i;
	}

	for (int i = 0; i < N; ++i){
		x[i] = x[i] + alpha *y[i];
	}

	printf("x = %d\n", x);

	return 0;
}