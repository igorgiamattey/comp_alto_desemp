#include "bitmap.h"
#include <stdlib.h>
#include <math.h>

void transform_to_bw (unsigned char * in, unsigned char * out, int size) {
	for (int i = 0; i < size; i++) {
		out[i] = (unsigned char) round( 0.11 * in[ 3*i + 0 ] + 
										 0.59 * in[ 3*i + 1 ] +
										 0.39 * in[ 3*i + 2 ] );
	}    
} 

__global__ void sobel_filter (unsigned char * in, unsigned char * out, int width, int height) {

	int Gx[9] = { -1,  0,  1, -2,  0,  2, -1,  0,  1 };
	int Gy[9] = { -1, -2, -1,  0,  0,  0,  1,  2,  1 };

	int i = blockDim.x * blockIdx.x + threadIdx.x;
	int j = blockDim.y * blockIdx.y + threadIdx.y;
	
	if (i >= 1 && i < height-1 && j >= 1 && j < width-1) {
		int dx = 0, dy = 0; 
		for( int x = 0; x < 3; x++ ) {
			for( int y = 0; y < 3; y++ ) {
				dx += in[(i+x-1)*width + j + y - 1]*Gx[3*x+y];
				dy += in[(i+x-1)*width + j + y - 1]*Gy[3*x+y];
			}       
		}
		
		float value = sqrtf( dx*dx + dy*dy );
		out[i*width + j] = value >= 0 ? (unsigned char)round(value) : 0;
	}

}

int main (int argc, char ** argv) {
	
	if( argc != 3 ) {
		fprintf(stderr, "Sobel Filter, use: %s <input bitmap image> <output bitmap image>\n", argv[0] );
		return 1;
	}
	
	BITMAPINFOHEADER header;
	unsigned char * img = bmp_load( (char *)argv[1], &header );
	int width = header.biWidth;
	int height = header.biHeight;
	int size = width * height;
	
	unsigned char * img_bw_host = (unsigned char *) malloc(size);
	unsigned char * img_sb_host = (unsigned char *) malloc(size);
	
	unsigned char *img_sb_devc, *img_bw_devc;
	cudaMalloc(&img_sb_devc, size);
	cudaMalloc(&img_bw_devc, size);
	
	dim3 threadsPerBlock(32, 32);
	dim3 blocksPerGrid((height + threadsPerBlock.x -1)/threadsPerBlock.x,(width + threadsPerBlock.y - 1)/threadsPerBlock.y);

	transform_to_bw( img, img_bw_host,  size );
	cudaMemcpy(img_bw_devc, img_bw_host, size, cudaMemcpyHostToDevice);

	sobel_filter<<< blocksPerGrid, threadsPerBlock >>>(img_bw_devc, img_sb_devc, width, height);

	cudaMemcpy(img_sb_host, img_sb_devc, size, cudaMemcpyDeviceToHost);
		
	bmp_save_bw((char *)argv[2], width, height, img_sb_host );
	
	free(img);
	free(img_sb_host);
	free(img_bw_host);
	cudaFree(img_bw_devc);
	cudaFree(img_sb_devc);
	
	return 0;
}