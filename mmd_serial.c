#include <sys/time.h>
double **a, **b, **c;

double matrixMult(int n)
{
int i, j, k;

   for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
         c[i][j] = 0.0;
         for (k=0; k<n; k++)
            c[i][j] += a[i][k] * b[k][j];
      }
   }
}  
/* end of matrixMult */

void print(int n)
{
int i, j;

   for (i=0; i<n; i++){
      for (j=0; j<n; j++)
         printf("%f ", c[i][j]);
      putchar('\n');
   }
}

int main(int argc, char *argv[])
{
int size, i, j;
 
   size = atoi(argv[1]);
  /* allocate a, b, and c to be n by n matrices */

   a = (double **)malloc(sizeof(double)*size);
   b = (double **)malloc(sizeof(double)*size);
   c = (double **)malloc(sizeof(double)*size);

   for (i=0; i<size; i++) {
      a[i] = (double*)malloc(sizeof(double)*size);
      b[i] = (double*)malloc(sizeof(double)*size);
      c[i] = (double*)malloc(sizeof(double)*size);
   } 

/* initialize a, b, c */

   for (i=0; i<size; i++) {
      for (j=0; j<size; j++) {
         a[i][j] = 2.0;
         b[i][j] = 5.0;
         c[i][j] = 0.0;
     }
   }
  matrixMult(size);
 
/* print results */
/* print(size);*/ 
   printf("%f\n", c[size-1][size-1]);
   free(a);
   free(b);
   free(c);
   exit(0);
} /* end of main */
