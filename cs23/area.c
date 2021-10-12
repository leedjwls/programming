#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

long millisecond() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return(1000*tv.tv_sec + tv.tv_usec/1000);
}

main(int argc, char *argv[]) {
   double x, y, dx, GFLOPS, start, end, elapsed;
   double a = 0.0;
   double b = 2.0;
   double area = 0.0, sum = 0.0;
   long N;
   int i, np, pid, dest, source, tag = 0;
   MPI_Status status;
 
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &np);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);

   if(argc != 2) {
      printf("usage: %s N\n", argv[0]);
      exit(1);
   }

   N = atol(argv[1]);
   
   dx = (b-a) / N;
   x = ((b-a) / np) * pid;
   
   start = millisecond();

   // calculates local area
   for(i = 0; i < N/np; i++) {
      x += dx;
      y = x/((x*x+2)*(x*x+2)*(x*x+2));
      area += y * dx;
   }
   // reduce local areas onto area on p0
   if(pid != 0) {
      MPI_Send(&area, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
   }
   else {
      sum = area;
      for(source = 1; source < np; source++) {
         MPI_Recv(&area, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);
         sum += area;
      }
   }   

   end = millisecond();

   elapsed = (double)(end - start);
   GFLOPS = (N/np * 12 / (elapsed/1000)) / 1000000000.0;
   if(pid == 0) {
      // print elapsed time and GFLOPS
      printf("area : %.10f\n", sum);
      printf("elapsed time : %.0f milliseconds\n", elapsed);
      printf("GFLOPS : %10.2lf\n", GFLOPS);
   }
   MPI_Finalize();
}
