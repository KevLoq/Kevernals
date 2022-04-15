#pragma once


// CUDA runtime
#include <cuda_runtime.h>

class ToyMatrixMultiplierCuda
{
public:
    ToyMatrixMultiplierCuda() = default;
    ~ToyMatrixMultiplierCuda() = default;

    int MatrixMultiply( int argc, char ** argv, int block_size, const dim3 & dimsA, const dim3 & dimsB );
};
