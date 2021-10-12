#include <stdio.h>
#include <stdlib.h>

main(int argc, char *argv[]) {
   int array, i, N, M, *A, *B, *C;
   N = atoi(argv[1]);
   M = atoi(argv[2]);
   A = (int *)malloc(sizeof(int) * M);
   B = (int *)malloc(sizeof(int) * M);
   C = (int *)malloc(sizeof(int) * M);
   omp_set_num_threads(N);
   #pragma omp parallel
   {
      #pragma omp for
      for(i=0; i<M; i++) {
         A[i] - i;
         B[i] = M - i;
      }
      #pragma omp for
      for(i=0; i<M; i++) {
         B[i] = M - i;
      }
      #pragma omp for
      for(i=0; i<M; i++) {
         C[i] = A[i] - B[i];
      }
   }
   for(i=0; i<M; i++) {
      printf("%d: %d %d %d\n", i,A[i],B[i],C[i]);
   }
}  

