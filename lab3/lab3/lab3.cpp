#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <string>
#include <vector>

int procId;
int numProcs;

void copy_string(char dest[10], std::string &source)
{
    for (int i = 0; i < source.size(); i++)
        dest[i] = source[i];
}

bool cmp_chr_str(char dest[], std::string source)
{
    for (int i = 0; i < source.size(); i++)
    {
        std::cout << dest[i] << " - " << source[i] << std::endl;
        if (dest[i] != source[i])
        {
            return false;
        }
    }
    return true;
}

struct student
{
    int id;
    float gpa;
    char name[10];
    char surname[10];
    student(int id, float gpa, std::string name, std::string surname)
    {
        this->id = id;
        this->gpa = gpa;
        memset(&this->name[0], 0, 10);
        memset(&this->surname[0], 0, 10);
        
        copy_string(this->name, name);
        copy_string(this->surname, surname);
        //std::cout << this->name << '-' << name << std::endl;
    };
    student() {};
    bool operator ==(const student &param)
    {
        std::cout << id << '-' << param.id << std::endl;
        if (id == param.id && strcmp(name, param.name) == 0 && strcmp(surname, param.surname) == 0)
            return true;
        return false;
    }
};

void testLab3(MPI_Aint studentType, const std::vector<student> &input, student searchedStud, bool expectedFound)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    MPI_Status status;
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
            if (founds[i] == true && expectedFound == true)
            {
                std::cout << "SUCCESS";
                return;
            }
        if (expectedFound == false)
            std::cout << "SUCCESS";
        else
            std::cout << "FAIL";
    }

}

void main(int argc, char *argv[])
{
    int blocklengths[4] = { 1, 1, 10, 10 };
    MPI_Datatype types[4] = { MPI_INT, MPI_FLOAT, MPI_CHAR, MPI_CHAR };
    MPI_Aint displacements[4] = { offsetof(student, id), offsetof(student, gpa), offsetof(student, name), offsetof(student, surname) };
    MPI_Datatype studentType;

    MPI_Init(&argc, &argv);
    MPI_Type_create_struct(4, blocklengths, displacements, types, &studentType);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Type_commit(&studentType);

    std::vector<student> students;
    students.reserve(5);

    students.push_back(student(1, 2, "Ristea1", "Stefan1"));
    students.push_back(student(1, 2, "Ristea2", "Stefan2"));
    students.push_back(student(1, 2, "Ristea3", "Stefan3"));
    students.push_back(student(1, 2, "Ristea4", "Stefan4"));

    student stud(1, 2, "Ristea1", "Stefan1");

    testLab3(studentType, students, stud, true);
    MPI_Finalize();
}