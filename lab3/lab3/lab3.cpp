#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <string>
#include <vector>

int procId;
int numProcs;

struct student
{
    int id;
    char name[10];
    char surname[10];
    student(int id, const std::string &name, const std::string &surname)
    {
        this->id = id;
        memset(&this->name[0], 0, 10);
        memset(&this->surname[0], 0, 10);
        strcpy_s(this->name, name.c_str());
        strcpy_s(this->surname, surname.c_str());
    };
    student() {};
};

inline bool operator==(const student& lhs, const student& rhs) 
{
    if (lhs.id == rhs.id && strcmp(lhs.name, rhs.name) == 0 && strcmp(lhs.surname, rhs.surname) == 0)
        return true;
    return false;
}

void testLab3(const MPI_Aint &studentType, const std::vector<student> &input, const student &searchedStud, bool expectedFound)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    std::vector<student> localInput;

    int procChunkSize = input.size() / numProcs;
    localInput.resize(procChunkSize);
    MPI_Scatter(&input[0], procChunkSize, studentType, &localInput[0], procChunkSize, studentType, 0, MPI_COMM_WORLD);

    int localFound = false;
    for (int i = 0; i < procChunkSize; i++)
    {
        if (searchedStud == localInput[i])
            localFound = true;
    }

    std::vector<int> founds;
    founds.resize(numProcs);
    MPI_Gather(&localFound, 1, MPI_INT, &founds[0], 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (procId == 0)
    {
        for (int i = 0; i < numProcs; i++)
        {
            if (founds[i] == true && expectedFound == true)
            {
                std::cout << "SUCCESS";
                return;
            }
        }
        if (expectedFound == false)
        {
            std::cout << "SUCCESS";
        }
        else
        {
            std::cout << "FAIL";
        }
    }
}

void main(int argc, char *argv[])
{
    int blocklengths[3] = { 1, 10, 10 };
    MPI_Datatype types[3] = { MPI_INT, MPI_CHAR, MPI_CHAR };
    MPI_Aint displacements[3] = { offsetof(student, id), offsetof(student, name), offsetof(student, surname) };
    MPI_Datatype studentType;

    MPI_Init(&argc, &argv);
    MPI_Type_create_struct(3, blocklengths, displacements, types, &studentType);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Type_commit(&studentType);

    std::vector<student> students;
    students.reserve(5);

    students.push_back(student(1, "Ristea1", "Stefan1"));
    students.push_back(student(1, "Ristea2", "Stefan2"));
    students.push_back(student(1, "Ristea3", "Stefan3"));
    students.push_back(student(1, "Ristea4", "Stefan4"));

    student stud(1, "Ristea4", "Stefan4");

    testLab3(studentType, students, stud, true);
    MPI_Finalize();
}