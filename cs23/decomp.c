#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define N 12

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
   float A[N][N], **local_A;
   int local_N, i, j, np2, np, pid, source, dest, tag = 0;
   MPI_Datatype vector_t;
   MPI_Status status;
   
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);
   MPI_Comm_size(MPI_COMM_WORLD, &np2);

   np = sqrt(np2);
   local_N = N/np;

   local_A = malloc_2d(local_N, local_N);

   //initialize of A and print
   if (pid == 0) {
      for (i=0; i<N; i++) {
         for (j=0; j<N; j++) {
            A[i][j] = (i*N+j)*0.001;
            printf("%6.3f ", A[i][j]);
         }
         printf("\n");
      }
      printf("------------------------------------------------\n");
   }

   //(i) decompose A into local_A
   MPI_Type_vector(N/np, N/np, N, MPI_FLOAT, &vector_t);
   MPI_Type_commit(&vector_t);

   if(pid == 0) {
      for(i=0; i<local_N; i++) {
         for(j=0; j<local_N; j++) {
            local_A[i][j] = A[i][j];
         }
      }
      i=0, j=0;
      for(dest=1; dest<np2; dest++) {
         if(dest%np == 0) {
            i += local_N;
            j = 0;
         }
         else j += local_N;
         MPI_Send(&(A[i][j]), 1, vector_t, dest, tag, MPI_COMM_WORLD);
      }
   }
   else MPI_Recv(&(local_A[0][0]), local_N*local_N, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &status);

   //(ii) local_A = local_A + pid
   for (i=0; i<local_N; i++)
      for (j=0; j<local_N; j++)
         local_A[i][j] += pid;

   //(iii) compose local_A to A
   if(pid != 0) {
      MPI_Send(&(local_A[0][0]), local_N*local_N, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
   }
   else {
      for(i=0; i<local_N; i++) {
         for(j=0; j<local_N; j++) {
            A[i][j] = local_A[i][j];
         }
      }
      i=0, j=0;
      for(source=1; source<np2; source++) {
         if(source%np == 0) {
            i += local_N;
            j = 0;
         }
         else j += local_N;
         MPI_Recv(&(A[i][j]), 1, vector_t, source, tag, MPI_COMM_WORLD, &status);
      }
   }

   // check the result 
   if (pid == 0)
      for (i=0; i<N; i++) {
         for (j=0; j<N; j++)
               printf("%6.3f ", A[i][j]);
         printf("\n");
      }   
   free(local_A);
   
   MPI_Finalize();
}
