
#include <mpi.h>
#include <iostream>
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
        std::cout << "insert the size of the array: \n";
        std::cin >> size;
        std::cout << "size is : " << size << "\n";
        for (int i = 0; i < size; i++)
        {
            std::cin >> arr[i];
        }
        std::cout << "The vector is : \n";
        for (int i = 0; i < size; i++)
        {
            std::cout << arr[i];
        }
        std::cout << "\nThe number you want to search is : \n";
        std::cin >> nr;
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
    int found = false;

    for (int i = end - 1; i >= start; i--)
    {
        if (arr[i] == nr)
        {
            MPI_Reduce(&i, &maxPos, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
            found = true;
            break;
        }
    }   

    if (found == false)
    {
        int i = -1;
        MPI_Reduce(&i, &maxPos, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    }

    if (procId == 0)
    {
        if (maxPos != -1)
        {
            std::cout << "the maximum position is : " << maxPos;
        }
        else
        {
            std::cout << "the number does not exist";
        }
    }

    MPI_Finalize();
}