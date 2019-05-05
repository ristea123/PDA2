__kernel void vector_add(__global const int *A, __global const int *B, __global int *C, __global const int *k) 
{
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
    int j = get_global_id(1);

    int sizeI = get_global_size(0);
    int sizeJ = get_global_size(1);

    for(int it = 0 ; it < *k ; it++)
        C[i * sizeJ + j] += A[i * k[0] + it] * B[sizeJ * it + j];
}