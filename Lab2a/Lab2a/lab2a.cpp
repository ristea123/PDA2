
#include <mpi.h>
#include <iostream>
#include <vector>
#define MASTER 0

int numProcs;

void verifyLab2a(const std::vector<int> input, int nrToSearch, int expectedPos)
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

    MPI_Bcast(&localInput[0], localInput.size(), MPI_INT, MASTER, MPI_COMM_WORLD);

    int avgProcChunkSize = localInput.size() / numProcs;
    int chunkStart = avgProcChunkSize * procId;
    int chunkEnd = chunkStart + avgProcChunkSize;
    if (procId == numProcs - 1)
    {
        chunkEnd += localInput.size() % numProcs;
    }

    int localMaxPos = -1;

    for (int i = chunkEnd - 1; i >= chunkStart; i--)
    {
        if (localInput[i] == nrToSearch)
        {
            localMaxPos = i;
            break;
        }
    }

    int maxPos;
    MPI_Reduce(&localMaxPos, &maxPos, 1, MPI_INT, MPI_MAX, MASTER, MPI_COMM_WORLD);
    if (procId == MASTER)
    {
        if (maxPos == expectedPos)
            std::cout << "SUCCESS" << std::endl;
        else
            std::cout << "FAIL" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    
    // Basic test.
    verifyLab2a({ 1, 2, 3, 4 }, 3, 2);
    // Chunks contains same item twice.
    verifyLab2a({ 1, 1, 2, 3, 4, 5, 6, 7 }, 1, 1);
    // Last chunk is bigger.
    verifyLab2a({ 1, 2, 3, 4, 5 }, 5, 4);
    MPI_Finalize();
}