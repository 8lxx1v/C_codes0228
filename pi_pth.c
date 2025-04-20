#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
pthread_t threads[64];

int n, nprocess;
double sum=0.0;

double f(double a){ return (4.0/(1.0+a*a));}

void cal(i){
  int j=0;
  double temp = 0.0;
  double x,h;
  h = 1.0/(double)n;
  for(j=i+1; j<=n;j+=nprocess){
    x = h*((double)j-0.5);
    temp += f(x);
  }
  sum += h*temp;
  pthread_exit(0);
}

int main(int argc, char* argv[]){
  double PI25DT = 3.141592653589793238462643;
  int i =0;
  n = atoi(argv[2]);
  nprocess = atoi(argv[1]);
  for(i=0;i<nprocess;i++){
    pthread_create(&threads[i], NULL, (void *)cal, (void*)i);
  }  
  printf("Estimation of pi is %.16f, Error is %.16f\n", sum, fabs(sum-PI25DT));
  return 0;
}
