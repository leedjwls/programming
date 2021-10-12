#include <stdio.h>
#include <stdlib.h>

main(int argc, char *argv[]) {
   int i, N, M, *A, *B, *C;

   if(argc != 3) {
      fprintf(stderr, "argument error\n");
      exit(1);
   }

   N = atoi(argv[1]);
   M = atoi(argv[2]);

   A = (int *)malloc(sizeof(int) * M);
   B = (int *)malloc(sizeof(int) * M);
   C = (int *)malloc(sizeof(int) * M);

   omp_set_num_threads(N);

   #pragma omp parallel for
   for(i=0; i<M; i++) {
      A[i] - i;
      B[i] = M - i;
   }
   #pragma omp parallel for
   for(i=0; i<M; i++) {
      C[i] = A[i] + B[i];

   for(i=0; i<M; i++) {
      printf("%d\n", C[i]);
      fprintf(stdout, "%d\n", C[i]);
   }

   free(A);
   free(B);
   free(C);
}

