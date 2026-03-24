#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define IDX(i, j) (i*n + j)
#define BS 72

void naive_gemm( double * C, double * A, double * B, double alpha, double beta, int n ) {   
	#pragma omp parallel for
	for( int i = 0; i < n; i++ ) {
		for( int j = 0; j < n; j++ ) {
			double s = 0.0;
			for( int k = 0; k < n; k++ ) {
				s += A[ IDX(i,k) ] * B[ IDX(k,j) ];
			}
			C[ IDX(i,j) ] = alpha * s + beta * C[ IDX(i,j) ];
		}
	}
}

void pro_gemm( double * C, double * A, double * B, double alpha, double beta, int n ) {   
	double * Bt = malloc( n * n * 8 );

	#pragma omp parallel for
	for( int i = 0; i < n; i++ ) {
		for( int j = 0; j < n; j++ ) {
			Bt[ IDX(i,j) ] = B[ IDX(j,i) ];
		}
	}

	#pragma omp parallel for
	for( int i = 0; i < n; i++ ) {
		for( int j = 0; j < n; j++ ) {
			double s = 0.0;
			for( int k = 0; k < n; k++ ) {
				s += A[ IDX(i,k) ] * Bt[ IDX(j, k) ];
			}
			C[ IDX(i,j) ] = alpha * s + beta * C[ IDX(i,j) ];
		}
	}

	free( Bt );
}

void pro_gemm_tile( double * C, double * A, double * B, double alpha, double beta, int n ) {   
	double * Bt = malloc( n * n * 8 );

	#pragma omp parallel for
	for( int i = 0; i < n; i++ ) {
		for( int j = 0; j < n; j++ ) {
			Bt[ IDX(i,j) ] = B[ IDX(j,i) ];
		}
	}

	#pragma omp parallel for
	for( int i = 0; i < n*n; i++ ) {
		C[ i ] = beta * C[ i ];
	}
	
	#pragma omp parallel for
	for( int i = 0; i < n; i += BS ) {
		for( int j = 0; j < n; j += BS ) {
			for( int k = 0; k < n; k += BS ) {
				for( int ii = i; ii < i+BS && ii < n; ii++) {
					for( int jj = j; jj < j + BS && jj < n; jj ++ ) {
						double s = 0.0;
						for( int kk = k; kk < k + BS && kk < n; kk++ ) {
							s += A[IDX(ii, kk) ] * Bt[ IDX(jj, kk) ]; 
						}
						C[ IDX(ii,jj) ] += alpha * s;
					}
				}
			}
		}
	}

	free( Bt );
}

void printm( double * A,  int n ) {
	for( int i = 0; i < n; i++ ) {
		for( int j = 0; j < n; j++ ) {
			printf("%.2f\t", A[IDX(i,j)] );
		}
		printf("\n");
	}
	printf("\n");
}

int main(int argc, char ** argv ) {
	int n = atoi( argv[1] );

	double * A = malloc( n * n * 8 );
	double * B = malloc( n * n * 8 );
	double * C = malloc( n * n * 8 );

	for( int i = 0; i < n; i++ ) {
		for( int j = 0; j < n; j++ ) {
			A[IDX(i,j)] = B[IDX(i,j)] = C[IDX(i,j)] = 1.0;
		}
	}

	pro_gemm_tile( C, A, B, 2.0, 2.0, n );
	//printm( C, n );

	free( A );
	free( B );
	free( C );

	return 0;

}