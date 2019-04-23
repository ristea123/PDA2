#include "mpi.h"
#include <stdio.h>
#include "iostream"
#include "vector"
#define NPROCS 8
using namespace std;

std::vector<int> a = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,12, 13, 14, 15, 16 };

int main(int argc, char *argv[])
{
    int rank, new_rank, sendbuf, recvbuf, numtasks;
    int ranks1[2] = { 0,1 };
    int ranks2[2] = { 2,3 };
    int ranks3[2] = { 4,5 };
    int ranks4[2] = { 6,7 };
    MPI_Group orig_group, new_group;
    MPI_Comm new_comm;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    if (numtasks != NPROCS) {
        printf("Must specify MP_PROCS= %d. Terminating.\n", NPROCS);
        MPI_Finalize();
        return 0;
    }
    sendbuf = rank;
    MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
    if (rank < 2)
    {
        MPI_Group_incl(orig_group, NPROCS / 4, ranks1, &new_group);
    }
    else
        if (rank < 4)
        {
            MPI_Group_incl(orig_group, NPROCS / 4, ranks2, &new_group);
        }
        else
            if (rank < 6)
            {
                MPI_Group_incl(orig_group, NPROCS / 4, ranks3, &new_group);
            }
            else
                MPI_Group_incl(orig_group, NPROCS / 4, ranks4, &new_group);
    MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);
    MPI_Allreduce(&sendbuf, &recvbuf, 1, MPI_INT, MPI_SUM, new_comm);
    MPI_Group_rank(new_group, &new_rank);
    if (rank < 2)
    {
        int sum = 0;
        int recvSum;
        for (int i = new_rank * 8; i < new_rank * 8 + 8; i++)
            sum += a[i];
        if (new_rank == 1)
            MPI_Send(&sum, 1, MPI_INT, 0, 1, new_comm);
        else
        {
            MPI_Recv(&recvSum, 1, MPI_INT, 1, 1, new_comm, &status);
            sum += recvSum;
            cout << sum;
        }
    }
    else
        if (rank < 4)
        {
            int prod = 1;
            int recvProd;
            for (int i = new_rank * 8; i < new_rank * 8 + 8; i++)
                prod *= a[i];
            if (new_rank == 1)
                MPI_Send(&prod, 1, MPI_INT, 0, 1, new_comm);
            else
            {
                MPI_Recv(&recvProd, 1, MPI_INT, 1, 1, new_comm, &status);
                prod *= recvProd;
                cout << prod;
            }
        }
        else
            if (rank < 6)
            {
                int min = a[0];
                int recvMin;
                for (int i = new_rank * 8; i < new_rank * 8 + 8; i++)
                    min = min < a[i] ? min : a[i];
                if (new_rank == 1)
                    MPI_Send(&min, 1, MPI_INT, 0, 1, new_comm);
                else
                {
                    MPI_Recv(&recvMin, 1, MPI_INT, 1, 1, new_comm, &status);
                    min = recvMin < min ? recvMin : min;
                    cout << min;
                }
            }
            else
            {
                int max = a[0];
                int recvMax;
                for (int i = new_rank * 8; i < new_rank * 8 + 8; i++)
                    max = max > a[i] ? max : a[i];
                if (new_rank == 1)
                    MPI_Send(&max, 1, MPI_INT, 0, 1, new_comm);
                else
                {
                    MPI_Recv(&recvMax, 1, MPI_INT, 1, 1, new_comm, &status);
                    max = recvMax > max ? recvMax : max;
                    cout << max;
                }
            }
    MPI_Finalize();
}