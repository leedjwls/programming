#include <stdio.h>

#define N 24
#define THREADS 8

__global__ void reduce(float *A, float *results)
{
   __shared__ float sdata[THREADS];
   int i;
   unsigned int s;
   i = blockDim.x * blockIdx.x + threadIdx.x;
   sdata[threadIdx.x] = A[i];

   for(s = blockDim.x/2; s>0; s>>=1) {
      if(threadIdx.x < s)
         if(sdata[threadIdx.x] <= sdata[threadIdx.x + s])
            sdata[threadIdx.x] = sdata[threadIdx.x + s];
      __syncthreads();
   }
   if(threadIdx.x == 0) results[blockIdx.x] = sdata[0];
}

int main()
{
   float A[N], *A_d, *results, *results_d, result;
   int i;

   dim3 dimBlock(THREADS);
   dim3 dimGrid((N+dimBlock.x-1)/dimBlock.x);
   cudaSetDevice(0);

   for (i=0; i<N; i++)
      A[i] = N-i;
   A[3] = 2*N; A[N-3] = -N;

   cudaMalloc((void **) &A_d, sizeof(float)*N);
   cudaMemcpy(A_d, A, sizeof(float)*N, cudaMemcpyHostToDevice);

   cudaMalloc((void **) &results_d, dimGrid.x*sizeof(float));

   reduce<<<dimGrid, dimBlock>>>(A_d, results_d);

   // copy values from GPU memory to CPU memory
   results = (float*)malloc(dimGrid.x*sizeof(float));
   cudaMemcpy(results, results_d, dimGrid.x*sizeof(float), cudaMemcpyDeviceToHost);

   // add the computed value
   result = 999999.0;
   for (i=0; i<dimGrid.x; i++)
      if(result < results[i]) result = results[i];

   printf("%f\n", result);

   cudaFree(A_d);
   cudaFree(results_d);
}
