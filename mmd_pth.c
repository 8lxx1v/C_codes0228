// Matrix multiplication using pthreads usage on Solaris:
// Compile: cc(gcc) -O3 -o mmd_pth.exe mmd_pth.c -lpthread
// Run: ./mmd_pth.exe [threadnum <= 256] [matrixsize]
//
#include "pthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
int size;
int numthreads;
double **a, **b, **c;

pthread_t threads[256];
pthread_attr_t attr;
struct loop_args{
    int begin;
    int end;
} loop[256];

void matrix_init()
{
int i, j;

   for (i=0; i<size; i++)
      for (j=0; j<size; j++){
         a[i][j]=2.0;
         b[i][j]=5.0;
      }
}

void thread_mm(step)
struct loop_args *step;
{
int i, j, k;
   
   for (i=step->begin; i<step->end; i++)
      for (j=0; j<size; j++){
         c[i][j]=0.0;
         for (k=0; k<size; k++)
            c[i][j] += a[i][k] * b[k][j];
      }
   (void) pthread_exit(0);
}

void print()
{
int i, j;
   
   for (i=0; i<size; i++){
      for (j=0; j<size; j++)
         printf("%f ", c[i][j]);
      putchar('\n');
   }
}

int main(int argc, char *argv[])
{
int i, status, CS;
void *exit_status;   

   if(argc != 3){
      perror("Error: must has 2 arguments! ./mm_pth.exe 2 256 \n");
      exit(1);
   }
   numthreads = atoi(argv[1]);
   size = atoi(argv[2]);
/* allocate a, b, and c to be n by n matrices */
   a = (double **)malloc(sizeof(double)*size);
   b = (double **)malloc(sizeof(double)*size);
   c = (double **)malloc(sizeof(double)*size);
   for (i=0; i<size; i++) {
      a[i] = (double*)malloc(sizeof(double)*size);
      b[i] = (double*)malloc(sizeof(double)*size);
      c[i] = (double*)malloc(sizeof(double)*size);
   } 
   matrix_init();
   CS = size/numthreads;
   for (i=0; i<numthreads; i++){
      loop[i].begin = CS*i;
      loop[i].end = CS*(i+1);
   }
/* set global thread attributes */
//   pthread_attr_init(&attr);
//   pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
   for (i=0; i<numthreads; i++){
      status = pthread_create(&threads[i],NULL,(void *)thread_mm,(void *)&loop[i]);
      if (status != 0) {
         perror("pthread_create fail!\n");
         exit(1);
      }
   }
   for (i=0; i<numthreads; i++)
      (void) pthread_join(threads[i], &exit_status);
   
   printf("%f\n", c[size-1][size-1]);
   free(a);
   free(b);
   free(c);
   exit(0);
}
