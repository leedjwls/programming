#include <stdio.h>

#define N 128000
#define THREADS 128

__global__ void reduce(float *sum, float dx)
{
   __shared__ float sdata[THREADS];
   unsigned int s;
   float x;
   int i;
   i = blockDim.x * blockIdx.x + threadIdx.x;
   x = dx * i;
   sdata[threadIdx.x] = (x/((x*x+2)*(x*x+2)*(x*x+2)))*dx;

   for(s = blockDim.x/2; s>0; s>>=1) {
      if(threadIdx.x < s)
         sdata[threadIdx.x] += sdata[threadIdx.x + s];
      __syncthreads();
   }
   if(threadIdx.x == 0) sum[blockIdx.x] = sdata[0];
}

main(int argc, char *argv[]) {
   float *area, *area_d, dx, a, b, elapsed_time;
   int i;
   b = 2.0;
   a = 0.0;

   dim3 dimBlock(THREADS);
   dim3 dimGrid((N+dimBlock.x-1)/dimBlock.x);

   dx = (b-a) / N;
   
   cudaSetDevice(0);
   cudaEvent_t start, stop;
   cudaEventCreate(&start);
   cudaEventCreate(&stop);
   cudaEventRecord(start, 0);

   cudaMalloc((void **) &area_d, dimGrid.x*sizeof(float));

   reduce<<<dimGrid, dimBlock>>>(area_d, dx);

   cudaEventRecord(stop, 0);
   cudaEventSynchronize(stop);
   cudaEventElapsedTime(&elapsed_time, start, stop);

   area = (float*)malloc(dimGrid.x*sizeof(float));
   cudaMemcpy(area, area_d, dimGrid.x*sizeof(float), cudaMemcpyDeviceToHost);

   for(i=0; i<N/THREADS; i++) {
      area[0] += area[i];
   }
   
   printf("elapsed time: %f ms\n", elapsed_time);
   printf("result: %f\n", area[0]);

   cudaFree(area_d);
}
