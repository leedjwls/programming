#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define N 144

float **malloc_2d(int row, int col)
{
   float **A, *ptr;
   int len, i;

   len = sizeof(float *)*row + sizeof(float)*col*row;
   A = (float **)malloc(len);
   ptr = (float *)(A + row);
   for(i = 0; i < row; i++)
      A[i] = (ptr + col*i);
   return A;
}

main(int argc, char* argv[])
{
   float A[N][N], B[N][N], C[N][N];
   float **local_A, **local_B, **local_C;
   int local_N, *displs, *counts, i, j, n=0;
   int np2, np, pid;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);
   MPI_Comm_size(MPI_COMM_WORLD, &np2);

   np = sqrt(np2);
   local_N = N/np;

   local_A = malloc_2d(local_N, local_N);
   local_B = malloc_2d(local_N, local_N);
   local_C = malloc_2d(local_N, local_N);

   if (pid == 0) {
      for (i=0; i<N; i++) {
         for (j=0; j<N; j++) {
            A[i][j] = i*N+j;
            B[i][j] = N*N-i*N-j;
         }
      }
   }

   displs = (int*)malloc(sizeof(int)*np2);
   counts = (int*)malloc(sizeof(int)*np2);

   for(i = 0; i < np2; i++) {
      if(i == 0) {
         displs[i] = 0;
         counts[i] = local_N;
      }
      else {
         counts[i] = local_N;
         if(i%np == 0) n = i * local_N * local_N;
         else n += local_N;
      displs[i] = n;
      }
   }

   for(i=0; i<local_N; i++) {
      MPI_Scatterv(A[i], counts, displs, MPI_FLOAT, local_A[i], local_N, MPI_FLOAT, 0, MPI_COMM_WORLD);
      MPI_Scatterv(B[i], counts, displs, MPI_FLOAT, local_B[i], local_N, MPI_FLOAT, 0, MPI_COMM_WORLD);
   }

   for(i=0; i<local_N; i++) {
      for(j=0; j<local_N; j++) {
         local_C[i][j] = local_A[i][j] + local_B[i][j];
      }
   }

   for(i=0; i<local_N; i++)
      MPI_Gatherv(local_C[i], local_N, MPI_FLOAT, C[i], counts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);

   if (pid ==0)
      for (i=0; i<N; i++)
         for (j=0; j<N; j++)
            if (C[i][j] != (A[i][j]+B[i][j]))
               printf("%d %d\n", i, j);

   free(local_A);
   free(local_B);
   free(local_C);
   free(displs);
   free(counts);

   MPI_Finalize();
} 
