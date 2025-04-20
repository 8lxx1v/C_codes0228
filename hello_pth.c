// Hello using pthreads:
//     cc(gcc) -O3 -o hello_pth.exe hello_pth.c -lpthread
//     ./hello_pth [# of threads]                 
#include "pthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
int NTHREADS;
pthread_t threads[240];
pthread_attr_t attr;

void hello()
{
   printf("Hello World!\n");
   (void) pthread_exit(0);
}

int main(argc, argv)
int argc;
char *argv[];
{
int th_no;
int status;
void *exit_status;

   if(argc != 2){
      perror("Error: must has 1 argument! ./hello_pth.exe threadnum \n");
      exit(1);
   }
   NTHREADS = atoi(argv[1]);
//   pthread_attr_init(&attr);
//   pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
   for(th_no = 0; th_no < NTHREADS; th_no++){
      status = pthread_create(&threads[th_no], &attr, (void *)hello, NULL);
      if (status != 0) {
         perror("pthread_create fail!\n"); exit(1);
      }
   }
   for(th_no = 0; th_no < NTHREADS; th_no++)
      (void) pthread_join(threads[th_no], &exit_status);
   exit(0);
}
