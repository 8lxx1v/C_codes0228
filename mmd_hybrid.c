#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MASTER 0
int Matrix_Size;
double **a, **b, **c;

/* modify by g8357001@thu */
void openmp_code(int node_start,int node_end,int mpi_rank)
{
	int nthreads, tid, i,j,k, chunk;
	
	// a,b,c was initail by mpi
	
   	//Set default chunk
	chunk = 100;
		
	#pragma omp parallel shared(a,b,c,nthreads,chunk) private(i,tid)
	{
	   tid = omp_get_thread_num();
	   if (tid == 0){ // Only master thread does this	
   	      nthreads = omp_get_num_threads(); 
          // 計算 chunk size
	      //chunk = (node_end - node_start) / nthreads + 1;
	      printf ("Number of threads = [%d] chunk size = [%d] on CPU(mpi_rank) [%d]\n", nthreads,chunk, mpi_rank);
	}
    //	printf("Thread %d starting...\n",tid);
	
	#pragma omp for schedule(static,chunk)
	
	   for (i=node_start; i<=node_end; i++) {
              for (j=0; j<Matrix_Size; j++)  {
                 c[i][j]=0;
              for (k=0; k<Matrix_Size; k++)
	         c[i][j] += a[i][k] * b[k][j];
           }
        }	
       //	printf("Thread %d: c[%d]= %f\n",tid,i,c[i]);
	}  /* end of parallel section */
}

int main(int argc, char* argv[])
{
int i,j,k;   
int mpi_size, mpi_rank;
int node_size, node_start, node_end;
double time_start, time_end;
MPI_Status mpi_status;
   
   
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
   printf("CPU(mpi_rank)/(mpi_size) %d/%d  is ready.\n", mpi_rank, mpi_size);
   
   if (mpi_rank == MASTER) {
     if (argc != 2) {
       printf("please input matrix size，eg. %si 256\n",argv[0]);
       MPI_Finalize();
       return 1;
     }
   }
   
   Matrix_Size = atoi(argv[1]);
   if (Matrix_Size >= 8192 || Matrix_Size <= 2)
   {
      printf("please less then 8192, thanks! \n");
      MPI_Finalize();
      return 1;
   }
   time_start=MPI_Wtime();

   /* allocate a, b, and c to be n by n matrices */
   a = (double **)malloc(sizeof(double)*Matrix_Size);
   b = (double **)malloc(sizeof(double)*Matrix_Size);
   c = (double **)malloc(sizeof(double)*Matrix_Size);

   for (i=0; i<Matrix_Size; i++) {
      a[i] = (double*)malloc(sizeof(double)*Matrix_Size);
      b[i] = (double*)malloc(sizeof(double)*Matrix_Size);
      c[i] = (double*)malloc(sizeof(double)*Matrix_Size);
   } 
   
   /* 設定矩陣初始值 */
   if (mpi_rank == MASTER)
   {
      for (i=0; i<Matrix_Size; i++)
         for (j=0; j<Matrix_Size; j++)  {
            a[i][j]=2.0;
            b[i][j]=5.0;
         }
   }

   /* 計算每個 Node 要計算的起始值與終止值 */
   node_size  = Matrix_Size / mpi_size;
   node_start = mpi_rank * node_size;
   node_end   = (mpi_rank + 1) * node_size - 1;
   printf("CPU %d is calculate between (%4d~%4d) of %d\n", mpi_rank, node_start, node_end, Matrix_Size);
   time_start=MPI_Wtime();
   /* 傳送 a 矩陣資料 */
   if (mpi_rank == MASTER) {
      for (i=node_end+1; i<Matrix_Size; i++) {
         int rank = (int)(i/node_size);
         MPI_Send(&a[i][0], Matrix_Size, MPI_DOUBLE, rank, rank, MPI_COMM_WORLD);
      } 
   }
   else {
      for (i=node_start; i<=node_end; i++)  {
         MPI_Recv(&a[i][0], Matrix_Size, MPI_DOUBLE, MASTER, mpi_rank, MPI_COMM_WORLD, &mpi_status);
      }
   }
   
   /* 傳送 b 矩陣資料 */
   for (i=0; i<Matrix_Size; i++) {
      if (mpi_rank == MASTER)  {
         for (j=1; j<mpi_size; j++)  {
            MPI_Send(&b[i][0], Matrix_Size, MPI_DOUBLE, j, i, MPI_COMM_WORLD);
         }
      }
      else  {
         /* BUG                                          */
         /* for (j=1; j<mpi_size; j++)                   */
         /* {                                            */
         MPI_Recv(&b[i][0], Matrix_Size, MPI_DOUBLE, MASTER, i, MPI_COMM_WORLD, &mpi_status);
         /* }                                            */
      }
   }
   
   /* 計算 c = a * b */
   openmp_code(node_start, node_end, mpi_rank);
   //for (i=node_start; i<=node_end; i++) {
   //   for (j=0; j<Matrix_Size; j++)  {
   //      c[i][j]=0;
   //      for (k=0; k<Matrix_Size; k++)
   //	    c[i][j] += a[i][k] * b[k][j];
   //   }
   //}
   /* 接收由各個 node 的計算結果 */
   if (mpi_rank == MASTER) {
      for (i=node_end+1; i<Matrix_Size; i++)
         MPI_Recv(&c[i][0], Matrix_Size, MPI_DOUBLE, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &mpi_status);
   }
   else
   {
      for (i=node_start; i<=node_end; i++)
        MPI_Send(&c[i][0], Matrix_Size, MPI_DOUBLE, MASTER, i, MPI_COMM_WORLD);
   }

   if (mpi_rank == MASTER) {
     for (i=0; i<Matrix_Size; i++) {
         for (j=0; j<Matrix_Size; j++)   {
            a[i][j]=i+j;
            b[i][j]=i*j;
            //printf ( "c[%d][%d] =%f ",i,j,c[i][j]); 
         }
         //printf("\n");
    }
    printf("Result: c[%d][%d] = %f\t", Matrix_Size-1, Matrix_Size-1, c[Matrix_Size-1][Matrix_Size-1]);
    time_end=MPI_Wtime();
    printf("Execute Time: %f\n", time_end-time_start);
   }
   MPI_Finalize();
   return 0;
}
