/******************************************************************************
* FILE: mmd1_omp.c
* DESCRIPTION:  
*   OpenMp Example - Matrix Multiply - C Version
*   Demonstrates a matrix multiply using OpenMP. Threads share row iterations
*   according to a predefined chunk size.
* AUTHOR: Blaise Barney
* LAST REVISED: 06/28/05
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) 
{
int	tid, nthreads, i, j, k, chunk, size;
double **a, **b, **c;

size = atoi(argv[2]);
a = (double **) malloc(sizeof(double)*size);
b = (double **) malloc(sizeof(double)*size);
c = (double **) malloc(sizeof(double)*size);
for (i=0; i<size; i++){
  a[i] = (double*)malloc(sizeof(double)*size);
  b[i] = (double*)malloc(sizeof(double)*size);
  c[i] = (double*)malloc(sizeof(double)*size);
}

chunk=atoi(argv[1]);

#pragma omp parallel shared(a, b, c, nthreads, chunk) private(tid, i, j, k)
{
  tid = omp_get_thread_num();
  if (tid == 0)
    {
    nthreads = omp_get_num_threads();
    printf("Starting matrix multiple example with %d threads\n",nthreads);
   // printf("Initializing matrices...\n");
    }

//init matrix
for(i=0;i<size;i++){
  for(j=0;j<size;j++){
    a[i][j]=2.0;
    b[i][j]=5.0;
    c[i][j]=0;
  }
}

  /*** Do matrix multiply sharing iterations on outer loop ***/
  /*** Display who does which iterations for demonstration purposes ***/

  //printf("Thread %d starting matrix multiply...\n",tid);

  #pragma omp for schedule (static, chunk)
  for (i=0; i<size; i++)    
    {
    //printf("Thread=%d did row=%d\n",tid,i);
    for(j=0; j<size; j++)       
      for (k=0; k<size; k++)
        c[i][j] = a[i][k] * b[k][j];
    }

}// end of parallel region

/*** Print results ***/
//printf("******************************************************\n");
/*
printf("Result Matrix:\n");
for (i=0; i<size; i++)
  {
  for (j=0; j<size; j++) 
    printf("%6.2f   ", c[i][j]);
  printf("\n"); 
  }
*/
//printf("******************************************************\n");
//printf ("Done.\n");
   printf("%f\n", c[size-1][size-1]);
   free(a);
   free(b);
   free(c);
   exit(0);
}
