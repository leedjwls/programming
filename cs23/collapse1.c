#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define N 12

main(int argc, char* argv[])
{
   int np, pid, i, dest, source, tag = 0;
   float A[N];
   MPI_Status status;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &np);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);

   // initialize
   for (i = 0; i < N/2; i++)
      A[i] = pid*(N/2)+i;
   // collapse
   if (pid == 1)
      MPI_Send(A, N/2, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
   else if (pid == 0)
      MPI_Recv(A+N/2, N/2, MPI_FLOAT, 1, tag, MPI_COMM_WORLD, &status);
 
   // print
   if (pid == 0) {
      for (i = 0; i < N; i++)
         printf("%f ", A[i]);
      printf("\n");
   }
  
   MPI_Finalize();
}
