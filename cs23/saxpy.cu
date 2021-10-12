#include <stdio.h>
#include <math.h>
#define N 10000
#define THREADS 100

__global__ void vecAdd(float *A, float *B)
{
   int i = blockDim.x * blockIdx.x + threadIdx.x;
   float X, Y;
   X = 1.23;
   Y = 2.34;

   B[i] = A[i]*X;
   B[i] += Y;
}

int main()
{
   float A[N], B[N], B2[N], X, Y, *A_d, *B_d;
   int i;

   dim3 dimBlock(THREADS);
   dim3 dimGrid(N/dimBlock.x);

   cudaMalloc((void **)&A_d, sizeof(float)*N);
   cudaMalloc((void **)&B_d, sizeof(float)*N);

   for (i=0; i<N; i++)
      A[i] = i*2;
  
   X = 1.23;
   Y = 2.34;
   for (i=0; i<N; i++) {
      B2[i] = A[i]*X + Y; // B2 is used for checking
   }

   cudaMemcpy(A_d, A, N*sizeof(float), cudaMemcpyHostToDevice);

   vecAdd<<<dimGrid, dimBlock>>>(A_d, B_d);

   cudaMemcpy(B, B_d, N*sizeof(float), cudaMemcpyDeviceToHost);

   // check the results
   for (i=0; i<N; i++)
      if (fabs(B[i]-B2[i]) > 0.001)
         printf("%d: %f %f\n",i, B[i], B2[i]);
}
