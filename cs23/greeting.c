#include "mpi.h"
#include <stdio.h>
#include <string.h>

main(int argc, char* argv[])
{
   int np, pid, dest;
   int tag = 0;
   char message[100];
   MPI_Status status;

   char processor_name[80];
   int name_len;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &np);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);

   MPI_Get_processor_name(processor_name, &name_len);
   printf("%s, rank %d\n", processor_name, pid);

   if (pid != 0) {
      sprintf(message, "Greeting from process %d", pid);
      MPI_Send(message, strlen(message)+1, MPI_CHAR, 0,
         tag, MPI_COMM_WORLD);
   }
   else {
      MPI_Recv(message, 100, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
      printf("%d: %s\n", pid, message);
   }

   MPI_Finalize();
}
