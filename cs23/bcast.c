#include <stdio.h>
#include "mpi.h"

main(int argc, char* argv[])
{
   int np, pid, dest, source, tag = 0;
   float data;
   MPI_Status status;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &np);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);

   if (pid == 0) {
      data = 100.0;
      for(dest = 1; dest < np; dest++) {
         MPI_Send(&data, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
      }
   }
   else {
      MPI_Recv(&data, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &status);
   }
   printf("%f\n", pid+data);

   MPI_Finalize();
}
