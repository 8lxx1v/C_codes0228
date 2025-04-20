#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MASTER 0
int Matrix_Size;
double **a, **b, **c;

int min(int r, int s)
{
   if (r >= s) return s;
   else return r;
}

int main(int argc, char* argv[]){
/* �ŧi�ܼ� */
/*
   mpi_size�G�`�I�ƶq
   mpi_rank�G����`�I�s��
   node_size�G�C�Ӹ`�I�p�⪺�j�p
*/
int i, j, k;   
int mpi_size, mpi_rank;
int node_size, node_start, node_end;
double time_start, time_end;
MPI_Status mpi_status;
   
/* MPI ��l�ơA�^��MPI_COMM_WORLD����mpi_size�Ӹ`�I�A���b����o�Ӹ`�I����mpi_rank */
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
   
   if (mpi_rank == MASTER)
/* �ˬd�O�_����J�x�}�j�p�Ѽ�*/
      if (argc != 2)
      {
         printf("Please input matrix size�Aeg. mmdvary_mpi.exe 512\n");
         MPI_Finalize();
         return 1;
      }
/* Ū���x�}�j�p */
   Matrix_Size = atoi(argv[1]);
   if (Matrix_Size > 9601 || Matrix_Size < 3)
   {
      printf("Please input matrix size less then 9601, and large then 4, thanks! \n");
      MPI_Finalize();
      return 1;
   }

   time_start=MPI_Wtime();
/* allocate a, b, and c to be Matrix_Size by Matrix_Size matrices */
   a = (double **)malloc(sizeof(double)*Matrix_Size);
   b = (double **)malloc(sizeof(double)*Matrix_Size);
   c = (double **)malloc(sizeof(double)*Matrix_Size);

   for (i=0; i<Matrix_Size; i++) 
   {
      a[i] = (double*)malloc(sizeof(double)*Matrix_Size);
      b[i] = (double*)malloc(sizeof(double)*Matrix_Size);
      c[i] = (double*)malloc(sizeof(double)*Matrix_Size);
   } 
   printf("CPU%2d/%2d is ready.\n", mpi_rank, mpi_size);   
/* �]�w�x�}��l�� */
   if (mpi_rank == MASTER)
      for (i=0; i<Matrix_Size; i++)
         for (j=0; j<Matrix_Size; j++)
         {
            a[i][j]=2.0;
            b[i][j]=5.0;
         }
/* �p��C�� Node �n�p�⪺�_�l�ȻP�פ�� */
//   node_size  = Matrix_Size / mpi_size;

//   node_size  = Matrix_Size / mpi_size + ((Matrix_Size % mpi_size != 0) ? 1:0);
//   node_start = mpi_rank * node_size;
//   node_end   = (mpi_rank + 1) * node_size - 1;
//   if (node_end >= Matrix_Size) node_end = Matrix_Size - 1;

   node_size  = (int)ceil ((float) Matrix_Size / mpi_size);
   node_start = mpi_rank * node_size;
   node_end   = min((mpi_rank+1) * node_size-1, Matrix_Size-1);

   printf("CPU%2d calculates between(%4d~%4d)of%4d in%4d\n",mpi_rank,node_start,node_end,node_end-node_start+1,Matrix_Size);
//   time_start=MPI_Wtime();
/* �ǰe a �x�}��� */
   if (mpi_rank == MASTER)
      for (i=node_end + 1; i<Matrix_Size; i++)
      {
         int rank = (int)(i/node_size);
         MPI_Send(&a[i][0], Matrix_Size, MPI_DOUBLE, rank, rank, MPI_COMM_WORLD);
      } 
   else
      for (i=node_start; i<=node_end; i++)
         MPI_Recv(&a[i][0], Matrix_Size, MPI_DOUBLE, MASTER, mpi_rank, MPI_COMM_WORLD, &mpi_status);
   
/* �ǰe b �x�}��� */
   for (i=0; i<Matrix_Size; i++)
   {
      if (mpi_rank == MASTER)
         for (j=1; j<mpi_size; j++)
            MPI_Send(&b[i][0], Matrix_Size, MPI_DOUBLE, j, i, MPI_COMM_WORLD);
      else
//         BUG                                          
//         for (j=1; j<mpi_size; j++)
         MPI_Recv(&b[i][0], Matrix_Size, MPI_DOUBLE, MASTER, i, MPI_COMM_WORLD, &mpi_status);
   }   

/* �p�� c = a * b */
   for (i=node_start; i<=node_end; i++)
      for (j=0; j<Matrix_Size; j++)
      {
         c[i][j]=0;
         for (k=0; k<Matrix_Size; k++)
	    c[i][j] += a[i][k] * b[k][j];
      }

/* �����ѦU�� node ���p�⵲�G */
   if (mpi_rank == MASTER)
      for (i=node_end+1; i<Matrix_Size; i++)
         MPI_Recv(&c[i][0], Matrix_Size, MPI_DOUBLE, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &mpi_status);
   else
      for (i=node_start; i<=node_end; i++)
         MPI_Send(&c[i][0], Matrix_Size, MPI_DOUBLE, MASTER, i, MPI_COMM_WORLD);

   if (mpi_rank == MASTER)
   {
//      for (i=0; i<Matrix_Size; i++)
//      {
//         for (j=0; j<Matrix_Size; j++)
//            printf("%8.1f", c[Matrix_Size-1][Matrix_Size-1]);
//         printf("\n");
//      }
      printf("Result:c[%d][%d]=%f\t", Matrix_Size-1, Matrix_Size-1, c[Matrix_Size-1][Matrix_Size-1]);
      time_end=MPI_Wtime();
      printf("Execute Time: %f\n", time_end-time_start);
   }
   MPI_Finalize();
   return 0;
}
