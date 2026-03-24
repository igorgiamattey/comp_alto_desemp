#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

long long fibo (int n){
	if (n == 1 || n == 2){
		return 1;
	}
	
	return fibo(n-1) + fibo(n-2);
}

long long fibopar (long long n){
	if (n <= 2) return 1;
    if (n < 25) return fibo(n);

	long long i, j;
	
	#pragma omp task shared(i)
	i = fibopar(n-1);

	#pragma omp task shared (j)
	j = fibopar(n-2);
	
	#pragma omp taskwait
	return i + j;
}

int main() {
	
	#pragma omp parallel
	{
		#pragma omp single
		{
			long long f = fibopar(50);
			printf("f = %lld\n", f);
		}
	}
}