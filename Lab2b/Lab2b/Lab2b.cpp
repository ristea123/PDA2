// Lab2b.cpp : Defines the entry point for the console application.
//

#include <mpi.h>
#include <iostream>
#include <vector>
#define MASTER 0

int numProcs;

void verifyLab2b(const std::vector<int> input, int nrToSearch, std::vector<int> expectedPos)
{
    int procId;
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    std::vector<int> localInput;

    if (procId == MASTER) 
    {
        localInput = input;
    }
    else 
    {
        localInput.resize(input.size());
    }

    int procChunkSize = localInput.size() / numProcs;
    MPI_Scatter(&input[0], procChunkSize, MPI_INT, &localInput[0], procChunkSize, MPI_INT, MASTER, MPI_COMM_WORLD);

    int localPos = -1;


    for (int i = 0; i < procChunkSize; i++)
    {
        
        if (localInput[i] == nrToSearch)
        {
            localPos = procId * procChunkSize + i;
        }
    }
    std::vector<int> recvLocalPos;
    recvLocalPos.resize(numProcs);
    

    MPI_Gather(&localPos, 1, MPI_INT, &recvLocalPos[0], 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    if (procId == MASTER)
    {
        if (std::equal(recvLocalPos.begin(), recvLocalPos.end(), expectedPos.begin()))
            std::cout << "SUCCESS" << std::endl;
        else
            std::cout << "FAIL" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int procId, arr[100], size, nr, recNumbers[100];
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);

    verifyLab2b({ 1, 2, 3, 2, 4, 2, 5, 2 }, 2, {1 ,3 , 5, 7});

    verifyLab2b({ 1, 2, 3, 2 }, 2, {-1, 1, -1, 3 });

    MPI_Finalize();
}