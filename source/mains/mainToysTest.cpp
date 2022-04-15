#include "modules/toys/ToyMatrixMul.h"
//#include "modules/toys/ToySet.h"
//#include "modules/toys/ToysGeometryUtils.h"
//
//
//#include <cstdlib>
//#include <ctime>
#include <cuda.h>
#include <cuda_runtime.h>
//#include <vector>
//
//// Helper functions and utilities to work with CUDA
#include <cmath>
#include <helper_cuda.h>
#include <helper_functions.h>
#include <iostream>


int main( int argc, char * argv[] )
{
    printf( "[Matrix Multiply Using CUDA] - Starting...\n" );

    // This will pick the best possible CUDA capable device, otherwise
    // override the device ID based on input provided at the command line
    int dev = findCudaDevice( argc, (const char **)argv );
    std::cout << "device : " << dev << std::endl;
    int block_size = 32;

    dim3 dimsA( 50 * 2 * block_size, 50 * 2 * block_size, 1 );
    dim3 dimsB( 50 * 4 * block_size, 50 * 2 * block_size, 1 );


    printf( "MatrixA(%d,%d), MatrixB(%d,%d)\n", dimsA.x, dimsA.y, dimsB.x, dimsB.y );

    ToyMatrixMultiplierCuda mulitplier;

    int matrix_result = mulitplier.MatrixMultiply( argc, argv, block_size, dimsA, dimsB );


    std::cout << " ---  DONE  ---  ;)" << std::endl;

    do
    {
        std::cout << std::endl
                  << "Press any key to continue...";
    } while( std::cin.get() != '\n' );

    exit( matrix_result );

    return 0;
}
