#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <sys/time.h>
static struct timeval time_value1;
static struct timeval time_value2;
#define START_TIMER gettimeofday(&time_value1, (struct timezone*)0)
#define STOP_TIMER gettimeofday(&time_value2, (struct timezone*)0)
#define ELAPSED_TIME ((double) ((time_value2.tv_usec - time_value1.tv_usec)*0.001 + ((time_value2.tv_sec - time_value1.tv_sec)*1000.0)))
#define BUFSIZE 1024*1024
//#define BUFSIZE 8196*8196
#define MAX_ITER 20
int main (int argc, char *argv[]) 
{
char buffer[BUFSIZE];
int size , namelen;
int count;
int my_rank, num_procs;
int dest;
int tag = 0;
char processor_name[MPI_MAX_PROCESSOR_NAME];
char greeting[MPI_MAX_PROCESSOR_NAME+80];
MPI_Status status;
double seconds, bps, avg_latency;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Get_processor_name(processor_name, &namelen);

    //printf("From processor %d of %d on %s\n", my_rank, num_procs, processor_name);
    sprintf (greeting, "%s", processor_name);

    if (2 != num_procs) {
        fprintf (stderr, "mptest: this program requires exactly two processes.\n");
        exit(1);
    }
    if (0 < argc) { size = atoi(argv[1]); } 
    else { size = BUFSIZE; }

    if ((0 > size) || (BUFSIZE < size)) {
        fprintf (stderr, "mpitest: buffer size must be between 0 and %d.\n", BUFSIZE);
        exit (1);
    }
    if (0 == my_rank) {
        dest = 1;                 

	printf ("%s ", greeting);
	MPI_Recv(greeting, sizeof(greeting), MPI_CHAR, dest, tag, MPI_COMM_WORLD, &status);
	printf ("%s ", greeting);

        /* Get the comm links warmed up */
        for (count = 0; count < 10; count++) {
            MPI_Send(buffer, size, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
            MPI_Recv(buffer, size, MPI_CHAR, dest, tag, MPI_COMM_WORLD, &status);
        }
        MPI_Barrier(MPI_COMM_WORLD); 
        START_TIMER;
        for (count = 0; count < MAX_ITER; count++) {
            MPI_Send(buffer, size, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
            MPI_Recv(buffer, size, MPI_CHAR, dest, tag, MPI_COMM_WORLD, &status);
        }
        STOP_TIMER;
        seconds = ELAPSED_TIME;
        bps = (2.0 * 8.0 * size * MAX_ITER) / seconds;
	bps /= 1024.0;
        avg_latency = seconds / MAX_ITER;
/*      printf ("Elapsed time = %g micro sec.\n", seconds);
        printf ("Packet size = %d bytes.\n", size);
        printf ("Iterations = %d.\n", MAX_ITER);
*/
        printf ("\t%gMbps\t%gms\n", bps, avg_latency);
/*
        printf ("Average round trip latency = %g micro sec. ", avg_latency);
*/
    } else {
        dest = 0;

	MPI_Send(greeting, sizeof(greeting), MPI_CHAR, dest, tag, MPI_COMM_WORLD);

        /* Get the comm links warmed up */
        for (count = 0; count < 10; count++) {
            MPI_Recv(buffer, size, MPI_CHAR, dest, tag, MPI_COMM_WORLD, &status);
            MPI_Send(buffer, size, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD); 
        for (count = 0; count < MAX_ITER; count++) {
            MPI_Recv(buffer, size, MPI_CHAR, dest, tag, MPI_COMM_WORLD, &status);
            MPI_Send(buffer, size, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}
