#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>

using namespace cl;

template <std::size_t M, std::size_t N>
using Matrix = std::array<std::array<int, N>, M>;

std::vector<Platform> platforms;
cl_context_properties cps[3];
std::vector<Device> devices;
CommandQueue queue;

std::ifstream sourceFile("vector_adds_kernel.cl");
std::string sourceCode(
    std::istreambuf_iterator<char>(sourceFile),
    (std::istreambuf_iterator<char>()));
Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
Program program;


template <std::size_t M, std::size_t N, std::size_t O>
void testLab5_2(const Matrix <M,N> &input1, const Matrix <N,O> &input2, 
                const Matrix <M,O> &expectedResult, const Context &context)
{
    // Build program for these specific devices
    program.build(devices);

    // Make kernel
    Kernel kernel(program, "vector_add");

    // Create memory buffers
    Buffer bufferA = Buffer(context, CL_MEM_READ_ONLY, input1.size() * input1[0].size() * sizeof(int));
    Buffer bufferB = Buffer(context, CL_MEM_READ_ONLY, input2.size() * input2[0].size() * sizeof(int));
    Buffer bufferC = Buffer(context, CL_MEM_WRITE_ONLY, input1.size() * input2[0].size() * sizeof(int));
    Buffer bufferK = Buffer(context, CL_MEM_READ_ONLY, sizeof(int));

    // Copy lists A and B to the memory buffers
    queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, input1.size() * input1[0].size() * sizeof(int), &input1[0]);
    queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, input2.size() * input2[0].size() * sizeof(int), &input2[0]);

    int k = input2.size();
    queue.enqueueWriteBuffer(bufferK, CL_TRUE, 0, sizeof(int), &k);

    // Set arguments to kernel//
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);
    kernel.setArg(3, bufferK);

    queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(input1.size(), input2[0].size()), NDRange(1, 1));

    Matrix<M, O> result = { { {}, {}} };
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, 4 * sizeof(int), &result[0]);

    if (result == expectedResult)
        std::cout << "SUCCESS" << std::endl;
    else
        std::cout << "FAIL" << std::endl;
}

int main() 
{
    Platform::get(&platforms);
    cps[0] = CL_CONTEXT_PLATFORM;
    cps[1] = (cl_context_properties)(platforms[0])();
    cps[2] = 0;
    Context context(CL_DEVICE_TYPE_GPU, cps);
    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    queue = CommandQueue(context, devices[0]);
    program = Program(context, source);

    Matrix<2, 3> input1 = { {
    {1,2,3},
    {4,5,6}
    } };

    Matrix<3, 2> input2 = { {
    {7,8},
    {9,10},
    {11,12}
    } };

    Matrix<2, 2> expectedResult = { {
    {58, 64},
    {139, 154}
    } };

    testLab5_2(input1, input2, expectedResult, context);

    Matrix<2, 3> input3 = { {
    {1,2,3},
    {4,5,6}
    }};

    Matrix<3, 4> input4 = { {
    {7,8,9,10},
    {9,10,11,12},
    {11,12,13,14}
    } };

    Matrix<2, 4> expectedResult2 = { {
    {58, 64, 1, 1},
    {139, 154, 1, 1}
    } };

    testLab5_2(input3, input4, expectedResult2, context);

    return 0;
}