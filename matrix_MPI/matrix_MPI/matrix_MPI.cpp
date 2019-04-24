#include <mpi.h>
#include <iostream>
#include <vector>
#define MASTER 0

int procId;
int numProcs;

void testMatrix_MPI(std::vector<std::vector<int>> m1, std::vector<std::vector<int>> m2, 
                        std::vector<std::vector<int>> expectedResult)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    std::vector<std::vector<int>> localM2;
    std::vector<std::vector<int>> localM1;
    std::vector<std::vector<int>> result;
    int nrColM1;

    if (procId == MASTER)
    {
        localM2 = m2;
        localM1 = m1;
        nrColM1 = m1[0].size();
        result.resize(m1.size());
        for (int i = 0; i < result.size(); i++)
            result[i].resize(m2[0].size());
    }
    else
    {
        localM2.resize(m2.size());
        for (int i = 0; i < localM2.size(); i++)
            localM2[i].resize(m2[i].size());
    }
    for(int i = 0 ; i < localM2.size() ; i++)
        MPI_Bcast(&localM2[i][0], localM2[0].size(), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nrColM1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int avgRows = m1.size() / numProcs;
    if (procId == 0)
    {
        for (int i = 1; i < numProcs; i++)
        {
            int nrSendRows = avgRows;
            if (i == numProcs - 1)
                nrSendRows += m1.size() % numProcs;
            MPI_Send(&nrSendRows, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            for (int j = 0; j < nrSendRows; j++)
            {
                std::vector<int> recvSums;
                recvSums.resize(localM1.size());
                MPI_Send(&localM1[avgRows * i + j][0], localM1[0].size(), MPI_INT, i, 0, MPI_COMM_WORLD);

                MPI_Recv(&recvSums[0], localM1.size(), MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int currCol = 0;
                for (auto it : recvSums)
                {
                    result[i * avgRows + j][currCol] = it;
                    currCol++;
                }
            }
        }
    }
    else
    {
        int nrRecvRows;
        std::vector<int> recvLine;
        recvLine.resize(nrColM1);
        MPI_Recv(&nrRecvRows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < nrRecvRows; i++)
        {
            MPI_Recv(&recvLine[0], nrColM1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::vector<int> sums;
            sums.resize(localM2[0].size());
            for (int k = 0; k < localM2[0].size(); k++)
            {
                sums[k] = 0;
                for (int j = 0; j < nrColM1; j++)
                {
                    sums[k] += recvLine[j] * localM2[j][k];
                }
            }
            MPI_Send(&sums[0], localM2[0].size(), MPI_INT, 0, 0, MPI_COMM_WORLD);

        }
    }

    std::vector<int> row = m1[0];
    
    if (procId == 0)
    {
        for (int i = 0; i < localM2[0].size(); i++)
        {
            int line = 0;
            int sum = 0;
            for (auto it : row)
            {
                sum += it * localM2[line][i];
                line++;
            }
            result[0][i] = sum;
        }
        for (auto it : result)
        {
            for (auto it2 : it)
                std::cout << it2 << " ";
            std::cout << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    std::vector<std::vector<int>> m1 = { { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15}, {16, 17, 18} } };
    std::vector<std::vector<int>> m2 = { { {10, 11, 12, 1, 2, 3}, {13, 14, 15, 1, 2, 3}, {16, 17, 18, 1, 2, 3} } };

    testMatrix_MPI(m1, m2, m2);
    MPI_Finalize();
    return 0;
}