#include <mpi.h>
#include <iostream>
#include <array>
#include <vector>

int procId;
int numProcs;

struct Student
{
    int id = {0};
    std::array<char, 10> name = {};
    std::array<char, 10> surname = {};
};

inline bool operator==(const Student& lhs, const Student& rhs)
{
    return((lhs.id == rhs.id && lhs.name == rhs.name && lhs.surname == rhs.surname) == 0);
}

void testLab3(const MPI_Aint &StudentType, const std::vector<Student> &input, const Student &searchedStud, int expectedFound)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    std::vector<Student> localInput;

    int procChunkSize = input.size() / numProcs;
    localInput.resize(procChunkSize);
    MPI_Scatter(&input[0], procChunkSize, StudentType, &localInput[0], procChunkSize, StudentType, 0, MPI_COMM_WORLD);
    int localFound = std::find(localInput.begin(), localInput.end(), searchedStud) != localInput.end() ? 1 : 0;

    std::vector<int> founds;
    founds.resize(numProcs);
    MPI_Gather(&localFound, 1, MPI_INT, &founds[0], 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (procId == 0)
    {
        for (int i = 0; i < numProcs; i++)
        {
            if (founds[i] == 1 && expectedFound == 1)
            {
                std::cout << "SUCCESS" << std::endl;
                return;
            }
        }
        if (expectedFound == 0)
        {
            std::cout << "SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "FAIL" << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    int blocklengths[3] = { 1, 10, 10 };
    MPI_Datatype types[3] = { MPI_INT, MPI_CHAR, MPI_CHAR };
    MPI_Aint offsets[3] = { offsetof(Student, id), offsetof(Student, name), offsetof(Student, surname) };
    MPI_Datatype StudentType;

    MPI_Init(&argc, &argv);
    MPI_Type_create_struct(3, blocklengths, offsets, types, &StudentType);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Type_commit(&StudentType);

    std::vector<Student> students = 
    {
        {1, {"Ristea1"}, {"Stefan1"}},
        {2, {"Ristea2"}, {"Stefan2"}},
        {3, {"Ristea3"}, {"Stefan3"}},
        {4, {"Ristea45"}, {"Stefan45"}},
        {5, {"Ristea6"}, {"Stefan6"}},
        {6, {"Ristea7"}, {"Stefan8"}},
        {7, {"Ristea9"}, {"Stefan9"}},
        {8, {"Ristea10"}, {"Stefan10"}}
    };

    Student stud = {4, {"Ristea45"}, {"Stefan45"} };
    Student stud1 = { 1, {"Ristea12"}, {"Stefan12"} };

    testLab3(StudentType, students, stud, 1);
    testLab3(StudentType, students, stud1, 0);
    MPI_Finalize();
    return 0;
}