#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void print_layout(int np, int inp, int jnp, int pid, int elem)
{
   int *elems, *ptr, i, j, k, tag;
   MPI_Status status;

   if (pid == 0 ) {
      elems = (int *)malloc(sizeof(int)*np);
      elems[0] = elem;
      for (i=1; i<np; i++) {
         MPI_Recv(elems+i, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
      }
      // initial layout
      k = 0;
      for (i=0; i<inp; i++) {
         for (j=0; j<jnp; j++)
            printf("%4d ", k++);
          printf("\n");
      }
      printf("\n==>\n");
      // layout after shift
      ptr = elems;
      for (i=0; i<inp; i++) {
         for (j=0; j<jnp; j++)
            printf("%4d ", *ptr++);
          printf("\n");
      }
      free(elems);
   }
   else
      MPI_Send(&elem, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
}
