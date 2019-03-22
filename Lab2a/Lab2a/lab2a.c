
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define MASTER 0

int main(int argc, char *argv[])
{
	int procId, numProcs, arr[100], size, nr;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &procId);
	if (procId == MASTER)
	{
		printf("insert the size of the array: \n");
		fflush(stdout);
		scanf_s("%d", &size);
		printf("size is :%d \n", size);
		fflush(stdout);
		for (int i = 0; i < size; i++)
		{
			scanf_s("%d", &arr[i]);
			fflush(stdout);
		}
		printf("The vector is : \n");
		fflush(stdout);
		for (int i = 0; i < size; i++)
			printf("%d ", arr[i]);
		fflush(stdout);
		printf("\n");
		fflush(stdout);
		printf("The number you want to search is : \n");
		fflush(stdout);
		scanf_s("%d", &nr);
	}
	MPI_Bcast(&size, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&nr, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(arr, size, MPI_INT, MASTER, MPI_COMM_WORLD);
	int avgProcSize = size / numProcs;
	int start = procId * avgProcSize;
	int end = start + avgProcSize;
	if (procId == numProcs - 1)
	{
		end += size % numProcs;
	}
	int maxPos = -1;
	for (int i = start; i < end; i++)
	{
		if (arr[i] == nr)
		{
			MPI_Reduce(&i, &maxPos, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
		}		
	}
	if (procId == 0)
	{
		if (maxPos != -1)
			printf("the maximum position is : %d", maxPos);
		else
			printf("the number does not exist");
	}
	MPI_Finalize();
}