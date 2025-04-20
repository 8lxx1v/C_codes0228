#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double f(double a){ return (4.0/(1.0+a*a));}


main (int argc, char *argv[]) 
{	
    double PI25DT = 3.141592653589793238462643;
	int i,num_steps;
    double x, pi, sum = 0.0,step;
	num_steps=atoi(argv[1]);
    step = 1.0/(double) num_steps; 
#pragma omp parallel for reduction(+:sum) private(x) 
	for (i=1;i<= num_steps; i++){ 
		  x = ((double)i-0.5)*step; 
		  sum = sum + f(x); 
	} 
	pi = step * sum;
    printf("Estimation of pi is %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    return 0;
}
