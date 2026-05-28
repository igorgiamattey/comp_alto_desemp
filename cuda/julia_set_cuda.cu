#include "bitmap.h"
#include <stdio.h>
#include <omp.h>
#include <math.h>

class complex {
	public:
		double r;
		double i;
		__host__ __device__ complex( double r, double i ) : r(r), i(i) {}
		__device__ complex operator*( complex& x ) {
			return complex( r * x.r - i * x.i, r * x.i + i * x.r );
		}
		__device__ complex operator+( complex& x ) {
			return complex( r + x.r, i + x.i );
		}
		__device__ double abs() { return ( r*r + i*i ); }
};



__device__ double juliamap( int value, double _min, double _max, int size ) {
	return _min + value * (_max - _min) / size;
}
/**
 * Zn+1 = Zn**2 + c
 */ 

__device__ int julia( double x, double y, complex c, int max_iter, double max_abs_z ) {
	
	complex z( x, y );
	int iter = 0;
	
	while( z.abs() < max_abs_z && iter < max_iter ) {
		z = z * z + c;
		iter++;
	}
	
	return iter;
}


__global__ void julia_set( double xmin, 
				  double xmax, 
				  double ymin, 
				  double ymax,
				  int width,
				  int height,
				  complex c,
				  int max_iter,
				  double max_abs_z,
				  unsigned char * buf ) {
	
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	int j = blockDim.y * blockIdx.y + threadIdx.y;
	
	if ( i < height && j < width) {
		double x = juliamap( i, xmin, xmax, height );
		double y = juliamap( j, ymin, ymax, width );
		int color = julia( x, y, c, max_iter, max_abs_z );

		buf[ 3*(i * width + j) + 0 ] = (color & 0xF)*16;            // B 
		buf[ 3*(i * width + j) + 1 ] = ((color >> 2) & 0xF)*16;     // G
		buf[ 3*(i * width + j) + 2 ] = ((color >> 3) & 0xF )*16 ;   // R
	}
				  
				  
}      

int main() {
  int width = 1024; 
  int height = 1024;
  double xmin = -1.5; 
  double ymin = -1.5; 
  double xmax = 1.5;
  double ymax = 1.5; 
  int max_iter = 255;
  double max_abs_z = 64.;
  complex c( -0.7, 0.27015 );
  //complex c( 0.6, -1.47015 );
  
  unsigned char * h_buf = (unsigned char *) malloc( width * height * 3 );
  unsigned char * d_buf;
  cudaMalloc( &d_buf, width * height * 3 );
	
	dim3 threadsPerBlock(32, 32);
	dim3 blocksPerGrid((height + threadsPerBlock.x -1)/threadsPerBlock.x,(width + threadsPerBlock.y - 1)/threadsPerBlock.y);
	
	julia_set<<< blocksPerGrid, threadsPerBlock >>>( xmin, 
			   xmax, 
			   ymin, 
			   ymax,
			   width,
			   height,
			   c,
			   max_iter,
			   max_abs_z,
			   d_buf );

	cudaMemcpy(h_buf, d_buf, width*height*3, cudaMemcpyDeviceToHost);
	
	int ret = bmp_generator("a.bmp", width, height, h_buf);
}
