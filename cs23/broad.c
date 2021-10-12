#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

void print_layout();

main(int argc, char* argv[])
{
   int np, pid, pid_from, inp, jnp, source, dest, tag;

   MPI_Comm grid_comm;
   MPI_Comm comm_split;
   int dim_sizes[2], wrap_around[2], coord[2], reorder;
   int data, direct, shift;
   MPI_Status status;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);
   MPI_Comm_size(MPI_COMM_WORLD, &np);

   inp = sqrt(np); jnp = np/inp;

   dim_sizes[0] = inp;
   dim_sizes[1] = jnp;
   wrap_around[0] = wrap_around[1] = 0;

   MPI_Cart_create(MPI_COMM_WORLD, 2, dim_sizes, wrap_around, reorder, &grid_comm);
   MPI_Cart_coords(grid_comm, pid, 2, coord);

   MPI_Comm_split(MPI_COMM_WORLD, coord[0], pid, &comm_split);
   data = pid;
   MPI_Bcast(&data, 1, MPI_INT, coord[0], comm_split);
   print_layout(np, inp, jnp, pid, data);

   MPI_Finalize();
}
