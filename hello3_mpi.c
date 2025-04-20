#include "mpi.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[])
{
int rank, size;
int namelen, i;
char processor_name[MPI_MAX_PROCESSOR_NAME];
char greeting[MPI_MAX_PROCESSOR_NAME+80];
MPI_Status status;  

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Get_processor_name(processor_name, &namelen);
   sprintf(greeting, "Hello World! from processor %d of %d on host %s", rank, size, processor_name);
   if (rank==0)
   {
      printf("%s\n", greeting);
      for (i=1; i<size; i++)
      {
         MPI_Recv(greeting, sizeof(greeting), MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
         printf("%s\n", greeting);
      }
   }
   else
   {
      MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
   }
   MPI_Finalize();
   return 0;
}
