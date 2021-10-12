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

   data = pid + 100.0;

   if(pid != 0) {
      MPI_Send(&data, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
   }
   else {
      for(source = 1; source < np; source++) {
         MPI_Recv(&data, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
      }
   }

   if(pid == 0) printf("%f\n", data);

   MPI_Finalize();
}
