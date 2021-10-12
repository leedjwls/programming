#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define N 24

float **malloc_2d(); // 2d malloc
void decomp(); // 2d decomposition
void comp(); // // 2d composition
void init(); // initialize with 0.0
void mmult(); // matrix multiplication
void check(); // check the results

main(int argc, char* argv[])
{
   float A[N][N], B[N][N], C[N][N];
   float **local_A, **local_A2, **local_B, **local_C;
   int np2, np, pid, root, source, dest, local_N, local_N2;
   int direct, shift, i, j;

   MPI_Comm grid_comm, row_comm;
   int dim_sizes[2], wrap_around[2], coord[2], reorder;
   MPI_Status status;
   int tag;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);
   MPI_Comm_size(MPI_COMM_WORLD, &np2);

   np = sqrt(np2);
   local_N = N/np;
   local_N2 = local_N*local_N;
   direct = 0;
   shift = -1;
   tag = 0;
   root = 0;
   dim_sizes[0] = dim_sizes[1] = np;
   wrap_around[0] = wrap_around[1] = 1;

   local_A = malloc_2d(local_N, local_N);
   local_A2 = malloc_2d(local_N, local_N);
   //local_A2 is a backup of local_A
   local_B = malloc_2d(local_N, local_N);
   local_C = malloc_2d(local_N, local_N);
   
   //initializaton of A and B
   if (pid == 0)
      for (i=0; i<N; i++)
         for (j=0; j<N; j++) {
            A[i][j] = (i*N+j)*0.001;
            B[i][j] = (N*N-i*N-j)*0.001;
         }
   
   // 2d decomposition of A and B
   decomp(N, local_N, np, A, local_A);
   decomp(N, local_N, np, B, local_B);
   copy(local_N, local_A, local_A2); // backup local_A

   MPI_Cart_create(MPI_COMM_WORLD, 2, dim_sizes, wrap_around, reorder, &grid_comm);
   MPI_Cart_coords(grid_comm, pid, 2, coord);

   // cartesian communicator for shift B north
   MPI_Cart_shift(grid_comm, direct, shift, &source, &dest);
   
   // split communicator for broadcast Ai to row
   MPI_Comm_split(MPI_COMM_WORLD, coord[0], pid, &row_comm);
   
   init(local_N, local_C);
   for (i=0; i<np; i++) {
      root = (coord[0]++)%np;
      // step1: broadcast diag Ai to row 
      copy(local_N, local_A2, local_A);
      MPI_Bcast(&local_A[0][0], local_N2, MPI_FLOAT, root, row_comm);
 
      // step2: C += A * B
      mmult(local_N, local_A, local_B, local_C);
   
      // step3: shift B north
      MPI_Sendrecv_replace(&local_B[0][0], local_N2, MPI_FLOAT, dest, tag, source, tag, MPI_COMM_WORLD, &status);
   }
   // 2d composition of C
   comp(N, local_N, np, local_C, C);
   
   // check results
   if (pid == 0) check(N, A, B, C);
   
   free(local_A); free(local_B); free(local_C);
   MPI_Finalize();
}
