#pragma once

#include "modules/reconstruction/Projector.h"
#include "modules/reconstruction/CudaErrorHandler.h"

#include <vtkTIFFWriter.h>

#include <cuda.h> 
#include <cuda_runtime.h>
#include <device_functions.h>
#include <device_launch_parameters.h>
#include <iostream>
#include <thrust/device_vector.h>
#include <thrust/execution_policy.h>
#include <thrust/sort.h>

#define _DEBUGPROJECTION
#define _DEBUGPBACKROJECTION
#define LOGPROJECTION
#define LOGPROJECTIONINITERATIONS_


__forceinline __device__ bool floatEq( const float p_A, const float p_B, const float p_tolerance = 0.0000001F )
{
    if( p_tolerance < 0 )
    {
        return ( p_A - p_B ) < -p_tolerance && ( p_A - p_B ) > p_tolerance;
    }
    else
    {
        return ( p_A - p_B ) < p_tolerance && ( p_A - p_B ) > -p_tolerance;
    }
}

__forceinline __device__ bool floatSupEq( const float p_A, const float p_B, const float p_tolerance = 0.0000001F )
{
    return p_A > p_B || floatEq( p_A, p_B, p_tolerance );
}

__forceinline __device__ bool floatInfEq( const float p_A, const float p_B, const float p_tolerance = 0.0000001F )
{
    return p_A < p_B || floatEq( p_A, p_B, p_tolerance );
}

// Important: user of this function: you are responsible of the cuda free of p_alphas pointer
__forceinline __device__ int FindProjectionAlphas_d( const float3 p_rayDirectorVector, const float3 p_currentSourcePosition, const dim3 p_volumeDimension, const int p_maxNumberOfDimensions, const float p_floatTolerance, float * p_alphas )
{
    // Volume half length in floating voxel reference
    float3 volumeHalfLength;
    volumeHalfLength.x = static_cast<float>( p_volumeDimension.x ) / 2.F;
    volumeHalfLength.y = static_cast<float>( p_volumeDimension.y ) / 2.F;
    volumeHalfLength.z = static_cast<float>( p_volumeDimension.z ) / 2.F;

    // Calculate max and min alpha- and i-values.
    float3 alpha1;    // alpha corresponding to the bottom left front vertex
    alpha1.x = fabs( p_rayDirectorVector.x ) > p_floatTolerance ? ( -volumeHalfLength.x - p_currentSourcePosition.x ) / p_rayDirectorVector.x : 0.F;
    alpha1.y = fabs( p_rayDirectorVector.y ) > p_floatTolerance ? ( -volumeHalfLength.y - p_currentSourcePosition.y ) / p_rayDirectorVector.y : 0.F;
    alpha1.z = fabs( p_rayDirectorVector.z ) > p_floatTolerance ? ( -volumeHalfLength.z - p_currentSourcePosition.z ) / p_rayDirectorVector.z : 0.F;
    float3 alphaN;    // alpha corresponding to the top right back vertex
    alphaN.x = fabs( p_rayDirectorVector.x ) > p_floatTolerance ? ( volumeHalfLength.x - p_currentSourcePosition.x ) / p_rayDirectorVector.x : 0.F;
    alphaN.y = fabs( p_rayDirectorVector.y ) > p_floatTolerance ? ( volumeHalfLength.y - p_currentSourcePosition.y ) / p_rayDirectorVector.y : 0.F;
    alphaN.z = fabs( p_rayDirectorVector.z ) > p_floatTolerance ? ( volumeHalfLength.z - p_currentSourcePosition.z ) / p_rayDirectorVector.z : 0.F;


    float3 alphaMins;    // alpha corresponding to the bottom left front vertex
    alphaMins.x = fmin( alpha1.x, alphaN.x );
    alphaMins.y = fmin( alpha1.y, alphaN.y );
    alphaMins.z = fmin( alpha1.z, alphaN.z );
    float3 alphaMaxs;    // alpha corresponding to the top right back vertex
    alphaMaxs.x = fmax( alpha1.x, alphaN.x );
    alphaMaxs.y = fmax( alpha1.y, alphaN.y );
    alphaMaxs.z = fmax( alpha1.z, alphaN.z );


    // We take the highest of the mins to catch the incoming intersection alpha
    auto alphaMin = fmax( fmax( 0.F, alphaMins.x ), fmax( alphaMins.y, alphaMins.z ) );
    // We take the lowest of the maxs to catch the outgoing intersection alpha
    auto alphaMax = fmin( fmin( 1.F, alphaMaxs.x ), fmin( alphaMaxs.y, alphaMaxs.z ) );

    if( floatSupEq( alphaMin, alphaMax ) )
    {
#ifdef DEBUGPROJECTION
        printf( "Id[%d] alphaMin = %f is greater or equal to alphaMax = %f !!! \n", globalProjectionIndex, alphaMin, alphaMax );
#endif
        return;
    }

    // min and max indices in each direction
    dim3 minIndices, maxIndices;
    if( p_rayDirectorVector.x > 0 )
    {
        maxIndices.x = max( 0, min( static_cast<int>( p_volumeDimension.x ) - 1, static_cast<int>( volumeHalfLength.x + p_currentSourcePosition.x + alphaMax * p_rayDirectorVector.x ) ) );
        minIndices.x = max( 0, min( static_cast<int>( maxIndices.x ), static_cast<int>( p_volumeDimension.x ) - static_cast<int>( volumeHalfLength.x - p_currentSourcePosition.x - alphaMin * p_rayDirectorVector.x ) ) );
    }
    else
    {
        maxIndices.x = max( 0, min( static_cast<int>( p_volumeDimension.x ) - 1, static_cast<int>( volumeHalfLength.x + p_currentSourcePosition.x + alphaMin * p_rayDirectorVector.x ) ) );
        minIndices.x = max( 0, min( static_cast<int>( maxIndices.x ), static_cast<int>( p_volumeDimension.x ) - static_cast<int>( volumeHalfLength.x - p_currentSourcePosition.x - alphaMax * p_rayDirectorVector.x ) ) );
    }

    if( p_rayDirectorVector.y > 0 )
    {
        maxIndices.y = max( 0, min( static_cast<int>( p_volumeDimension.y ) - 1, static_cast<int>( volumeHalfLength.y + p_currentSourcePosition.y + alphaMax * p_rayDirectorVector.y ) ) );
        minIndices.y = max( 0, min( static_cast<int>( maxIndices.y ), static_cast<int>( p_volumeDimension.y ) - static_cast<int>( volumeHalfLength.y - p_currentSourcePosition.y - alphaMin * p_rayDirectorVector.y ) ) );
    }
    else
    {
        maxIndices.y = max( 0, min( static_cast<int>( p_volumeDimension.y ) - 1, static_cast<int>( volumeHalfLength.y + p_currentSourcePosition.y + alphaMin * p_rayDirectorVector.y ) ) );
        minIndices.y = max( 0, min( static_cast<int>( maxIndices.y ), static_cast<int>( p_volumeDimension.y ) - static_cast<int>( volumeHalfLength.y - p_currentSourcePosition.y - alphaMax * p_rayDirectorVector.y ) ) );
    }

    if( p_rayDirectorVector.z > 0 )
    {
        maxIndices.z = max( 0, min( static_cast<int>( p_volumeDimension.z ) - 1, static_cast<int>( volumeHalfLength.z + p_currentSourcePosition.z + alphaMax * p_rayDirectorVector.z ) ) );
        minIndices.z = max( 0, min( static_cast<int>( maxIndices.z ), static_cast<int>( p_volumeDimension.z ) - static_cast<int>( volumeHalfLength.z - p_currentSourcePosition.z - alphaMin * p_rayDirectorVector.z ) ) );
    }
    else
    {
        maxIndices.z = max( 0, min( static_cast<int>( p_volumeDimension.z ) - 1, static_cast<int>( volumeHalfLength.z + p_currentSourcePosition.z + alphaMin * p_rayDirectorVector.z ) ) );
        minIndices.z = max( 0, min( static_cast<int>( maxIndices.z ), static_cast<int>( p_volumeDimension.z ) - static_cast<int>( volumeHalfLength.z - p_currentSourcePosition.z - alphaMax * p_rayDirectorVector.z ) ) );
    }

    // Calculate alpha-sets for x, y and z.
    auto globalSize = ( maxIndices.x - minIndices.x )
                      + ( maxIndices.y - minIndices.y )
                      + ( maxIndices.z - minIndices.z )
                      + 3;    // each component goes from minIndices to maxIndices (included) than
    // it includes (maxIndices - minIndices + 1) elements

    if( globalSize > p_maxNumberOfDimensions )
    {
        // TODO : DIMENSION PROBLEM TO HANDLE
    }

    for( auto i = 0; i < globalSize; i++ )
    {
        p_alphas[i] = 1.0;
    }
    auto initialIndex = 0;
    auto finalComponentWiseIndex = initialIndex + ( maxIndices.x - minIndices.x ) + 1;

    // x intersections findings
    if( fabs( p_rayDirectorVector.x ) > p_floatTolerance )
    {
        const auto xAlphaDiff = 1.F / p_rayDirectorVector.x;
        if( p_rayDirectorVector.x > 0 )
        {
            p_alphas[initialIndex] = xAlphaDiff * ( -volumeHalfLength.x - p_currentSourcePosition.x + static_cast<float>( minIndices.x ) );
        }
        else
        {
            p_alphas[initialIndex] = xAlphaDiff * ( -volumeHalfLength.x - p_currentSourcePosition.x + static_cast<float>( maxIndices.x ) );
        }

        for( auto index = initialIndex + 1; index < finalComponentWiseIndex; index++ )
        {
            p_alphas[index] = p_alphas[index - 1] + fabs( xAlphaDiff );
        }
    }

    initialIndex = finalComponentWiseIndex;
    finalComponentWiseIndex += ( maxIndices.y - minIndices.y ) + 1;
    // y intersections findings
    if( fabs( p_rayDirectorVector.y ) > p_floatTolerance )
    {
        const auto yAlphaDiff = 1.F / p_rayDirectorVector.y;
        if( p_rayDirectorVector.y > 0 )
        {
            p_alphas[initialIndex] = yAlphaDiff * ( -volumeHalfLength.y - p_currentSourcePosition.y + static_cast<float>( minIndices.y ) );
        }
        else
        {
            p_alphas[initialIndex] = yAlphaDiff * ( -volumeHalfLength.y - p_currentSourcePosition.y + static_cast<float>( maxIndices.y ) );
        }

        for( auto index = initialIndex + 1; index < finalComponentWiseIndex; index++ )
        {
            p_alphas[index] = p_alphas[index - 1] + fabs( yAlphaDiff );
        }
    }

    initialIndex = finalComponentWiseIndex;
    finalComponentWiseIndex += ( maxIndices.z - minIndices.z ) - 1;

    // z intersections findings
    if( fabs( p_rayDirectorVector.z ) > p_floatTolerance )
    {
        const auto zAlphaDiff = 1.F / p_rayDirectorVector.z;
        if( p_rayDirectorVector.z > 0 )
        {
            p_alphas[initialIndex] = zAlphaDiff * ( -volumeHalfLength.z - p_currentSourcePosition.z + static_cast<float>( minIndices.z ) );
        }
        else
        {
            p_alphas[initialIndex] = zAlphaDiff * ( -volumeHalfLength.z - p_currentSourcePosition.z + static_cast<float>( maxIndices.z ) );
        }

        for( auto index = initialIndex + 1; index < finalComponentWiseIndex; index++ )
        {
            p_alphas[index] = p_alphas[index - 1] + fabs( zAlphaDiff );
        }
    }


    // SELECTION SORT
    for( auto i = 0; i < globalSize - 1; i++ )
    {
        // Find the minimum element in unsorted array
        auto min_idx = i;
        for( auto j = i + 1; j < globalSize; j++ )
        {
            if( p_alphas[j] < p_alphas[min_idx] )
            {
                min_idx = j;
            }
        }
        // Swap the found minimum element with the first element
        auto tmpFloat = p_alphas[min_idx];
        p_alphas[min_idx] = p_alphas[i];
        p_alphas[i] = tmpFloat;
    }

    return globalSize;
}

// For this procedure, the coordinate system is the one centered in C, the pave volume
// center.
__global__ void CudaPerformProjection(
  const float * p_volumeBuffer,
  const dim3 p_volumeDimension,
  const float3 p_volumeOriginInWorld,
  const float3 p_volumeVoxelsSpacing,
  float * p_projectionsBuffer,
  const dim3 p_projectionsDimension,
  const float2 p_projectionsPixelsSpacing,
  const float3 * p_projectionsOriginInWorld,
  const float3 * p_sourcesPositions )
{
    const auto nbProjections = p_projectionsDimension.z;
    const auto nPixelsInOneRoi = p_projectionsDimension.x * p_projectionsDimension.y;
    const auto nProjectionsPixels = nbProjections * nPixelsInOneRoi;
    const auto projectionIndex = blockIdx.z * blockDim.z + threadIdx.z;
    const auto xProjPixel = blockIdx.x * blockDim.x + threadIdx.x;
    const auto yProjPixel = blockIdx.y * blockDim.y + threadIdx.y;

    constexpr int maxNumberOfDimensions = 2000;
    constexpr auto floatTolerance = 0.000001F;

#ifdef DEBUGPROJECTION
    // debugging index for some useful logging
    const auto debuggingProjIndex = 6;
    const auto debuggingProjX = 23;
    const auto debuggingProjY = 15;
    const auto debuggingIndex = debuggingProjIndex * nPixelsInOneRoi + debuggingProjY * p_projectionsDimension.x + debuggingProjX;
#endif


    if( projectionIndex < 0 || projectionIndex >= nbProjections
        || xProjPixel < 0 || xProjPixel >= p_projectionsDimension.x
        || yProjPixel < 0 || yProjPixel >= p_projectionsDimension.y )
    {
        return;
    }

    const auto globalProjectionIndex = projectionIndex * nPixelsInOneRoi + yProjPixel * p_projectionsDimension.x + xProjPixel;

    if( globalProjectionIndex < 0 || globalProjectionIndex >= nProjectionsPixels )
    {
#ifdef DEBUGPROJECTION
        printf( "Id[%d] NOT POSSIBLE TO BE THERE !!! \n", globalProjectionIndex );
#endif
        return;
    }
    p_projectionsBuffer[globalProjectionIndex] = 0.F;

#ifdef DEBUGPROJECTION
    if( globalProjectionIndex == debuggingIndex )
    {
        printf( "___basic computations___\n Id[%d] p_projectionsDimension.x : %d\n p_projectionsDimension.y : %d\n nbProjections : %d\n nPixelsInOneRoi : %d\n nProjectionsPixels : %d\n blockIdx: %d , %d , %d\n blockDim: %d , %d , %d\n threadIdx: %d , %d , %d\n projectionIndex : %d\n xProjPixel : %d\n yProjPixel : %d\n_________\n",
                globalProjectionIndex,
                p_projectionsDimension.x,
                p_projectionsDimension.y,
                nbProjections,
                nPixelsInOneRoi,
                nProjectionsPixels,
                blockIdx.x,
                blockIdx.y,
                blockIdx.z,
                blockDim.x,
                blockDim.y,
                blockDim.z,
                threadIdx.x,
                threadIdx.y,
                threadIdx.z,
                projectionIndex,
                xProjPixel,
                yProjPixel );
    }
#endif


    // source positions (in the floating voxel coordinate system with same origin as world)
    // origin is the volume center
    float3 currentSourcePositionFloatingVoxel;
    currentSourcePositionFloatingVoxel.x = p_sourcesPositions[projectionIndex].x / p_volumeVoxelsSpacing.x;
    currentSourcePositionFloatingVoxel.y = p_sourcesPositions[projectionIndex].y / p_volumeVoxelsSpacing.y;
    currentSourcePositionFloatingVoxel.z = p_sourcesPositions[projectionIndex].z / p_volumeVoxelsSpacing.z;

    float3 currentProjectionPositionFloatingVoxel;

    currentProjectionPositionFloatingVoxel.x = ( p_projectionsOriginInWorld[projectionIndex].x + ( static_cast<float>( xProjPixel ) + 0.5F ) * p_projectionsPixelsSpacing.x ) / p_volumeVoxelsSpacing.x;
    currentProjectionPositionFloatingVoxel.y = ( p_projectionsOriginInWorld[projectionIndex].y + ( static_cast<float>( yProjPixel ) + 0.5F ) * p_projectionsPixelsSpacing.y ) / p_volumeVoxelsSpacing.y;
    currentProjectionPositionFloatingVoxel.z = p_projectionsOriginInWorld[projectionIndex].z / p_volumeVoxelsSpacing.z;

    float3 rayDirectorVectorFloatingVoxel;
    rayDirectorVectorFloatingVoxel.x = currentProjectionPositionFloatingVoxel.x - currentSourcePositionFloatingVoxel.x;
    rayDirectorVectorFloatingVoxel.y = currentProjectionPositionFloatingVoxel.y - currentSourcePositionFloatingVoxel.y;
    rayDirectorVectorFloatingVoxel.z = currentProjectionPositionFloatingVoxel.z - currentSourcePositionFloatingVoxel.z;

    // Volume half length in floating voxel reference
    float3 volumeHalfLength;
    volumeHalfLength.x = static_cast<float>( p_volumeDimension.x ) / 2.F;
    volumeHalfLength.y = static_cast<float>( p_volumeDimension.y ) / 2.F;
    volumeHalfLength.z = static_cast<float>( p_volumeDimension.z ) / 2.F;

    float alphas[maxNumberOfDimensions];
    const auto raySize = FindProjectionAlphas_d( rayDirectorVectorFloatingVoxel, currentSourcePositionFloatingVoxel, p_volumeDimension, maxNumberOfDimensions, floatTolerance, alphas );

    // What are the alphas ?
    // this is the parameter which is used to identify a point on a line whose director vector is known.
    // if SP-> is the director vector of line (SP), for any point M on (SP), there is alpha, such that    SM-> = alpha x SP->
    // We compute those alphas for the current ray and for the intersection points with the underlying grids


#ifdef DEBUGPROJECTION
    if( globalProjectionIndex == debuggingIndex )
    {
        printf( "_______ sorted _______\n" );
        for( auto i = 0; i < totalAlphasSize; i++ )
        {
            printf( "   %f \n", alphas[i] );
        }
    }
#endif

    // projection performance
    auto total{ 0.F };
    auto totalWeight{ 0.F };
    for( auto i = 0; i < raySize - 1; i++ )
    {
        const auto centerAlpha = ( alphas[i + 1] + alphas[i] ) / 2.0;

        const auto pixX = static_cast<int>( volumeHalfLength.x + currentSourcePositionFloatingVoxel.x + centerAlpha * rayDirectorVectorFloatingVoxel.x );
        const auto pixY = static_cast<int>( volumeHalfLength.y + currentSourcePositionFloatingVoxel.y + centerAlpha * rayDirectorVectorFloatingVoxel.y );
        const auto pixZ = static_cast<int>( volumeHalfLength.z + currentSourcePositionFloatingVoxel.z + centerAlpha * rayDirectorVectorFloatingVoxel.z );

        const auto weight = ( alphas[i + 1] - alphas[i] );

        if( ( pixX >= 0 ) && ( pixX < p_volumeDimension.x ) && ( pixY >= 0 ) && ( pixY < p_volumeDimension.y )
            && ( pixZ >= 0 ) && ( pixZ < p_volumeDimension.z ) && weight > floatTolerance )
        {
            const auto currentIntersectionVoxelIndex = pixZ * p_volumeDimension.x * p_volumeDimension.y
                                                       + pixY * p_volumeDimension.x
                                                       + pixX;
            totalWeight += weight;
            total += weight * p_volumeBuffer[currentIntersectionVoxelIndex];
        }
    }
    if( totalWeight > floatTolerance )
    {
        p_projectionsBuffer[globalProjectionIndex] = total / totalWeight;
    }
}

// For this procedure, the coordinate system is the one centered in C, the pave volume
// center.
__global__ void CudaPerformBackProjection(
  const float * p_projectionsBuffer,
  const dim3 p_volumeDimension,
  const float3 p_volumeOriginInWorld,
  const float3 p_volumeVoxelsSpacing,
  float * p_volumeBuffer,
  const dim3 p_projectionsDimension,
  const float2 p_projectionsPixelSpacing,
  const float3 * p_projectionsOriginInWorld,
  const float3 * p_sourcesPositions )
{
    dim3 currentVoxel;
    currentVoxel.x = blockIdx.x * blockDim.x + threadIdx.x;
    currentVoxel.y = blockIdx.y * blockDim.y + threadIdx.y;
    currentVoxel.z = blockIdx.z * blockDim.z + threadIdx.z;

    if( currentVoxel.x < 0 || currentVoxel.x >= p_volumeDimension.x
        || currentVoxel.y < 0 || currentVoxel.y >= p_volumeDimension.y
        || currentVoxel.z < 0 || currentVoxel.z >= p_volumeDimension.z )
    {
        return;
    }

    const auto volumeSize{ p_volumeDimension.x * p_volumeDimension.y * p_volumeDimension.z };

    const auto volumeIndex = currentVoxel.z * p_volumeDimension.x * p_volumeDimension.y + currentVoxel.y * p_volumeDimension.x + currentVoxel.x;
    if( volumeIndex < 0 || volumeIndex >= volumeSize )
    {
#ifdef DEBUGPBACKROJECTION
        printf( "Id[%d] NOT POSSIBLE TO BE THERE !!! \n", volumeIndex );
#endif
        return;
    }


    const auto nbProjections = p_projectionsDimension.z;
    const auto nPixelsInOneRoi = p_projectionsDimension.x * p_projectionsDimension.y;
    const auto nProjectionsPixels = nbProjections * nPixelsInOneRoi;

    constexpr int maxNumberOfDimensions = 2000;
    constexpr auto floatTolerance = 0.000000000000000000000001F;

#ifdef DEBUGPBACKROJECTION
    // debugging index for some useful logging
    dim3 debuggingVoxel;
    debuggingVoxel.x = 329;
    debuggingVoxel.y = 494;
    debuggingVoxel.z = 20;
    const auto debuggingIndex = debuggingVoxel.z * p_volumeDimension.x * p_volumeDimension.y + debuggingVoxel.y * p_volumeDimension.x + debuggingVoxel.x;
    /*debuggingVoxel.x = 329;
    debuggingVoxel.y = 495;
    debuggingVoxel.z = 20;
    const auto debuggingIndex2 = debuggingVoxel.z * p_volumeDimension.x * p_volumeDimension.y + debuggingVoxel.y * p_volumeDimension.x + debuggingVoxel.x;*/
#endif

    p_volumeBuffer[volumeIndex] = 0.F;

    // coordinates in the FLOATING coordinate system (F-system) will all end with F
    // Origin in the F-system is the volume center.
    // unit is a voxelSpacing, dimension wise

    float3 currentVoxelF;
    currentVoxelF.x = static_cast<float>( currentVoxel.x ) - static_cast<float>( p_volumeDimension.x ) / 2.F;
    currentVoxelF.y = static_cast<float>( currentVoxel.y ) - static_cast<float>( p_volumeDimension.y ) / 2.F;
    currentVoxelF.z = static_cast<float>( currentVoxel.z ) - static_cast<float>( p_volumeDimension.z ) / 2.F;


#ifdef DEBUGPBACKROJECTION
    if( debuggingIndex == volumeIndex )
    {
        printf( "Id[%d]\t\t current voxel ( %d , %d , %d ) \n \t\t current voxel F ( %f , %f , %f ) \n", volumeIndex, currentVoxel.x, currentVoxel.y, currentVoxel.z, currentVoxelF.x, currentVoxelF.y, currentVoxelF.z );
    }
#endif

    auto total{ 0.F };
    auto totalWeight{ 0.F };
    for( auto projectionIndex{ 0 }; projectionIndex < nbProjections; projectionIndex++ )
    {
        float3 currentSourcePositionF;
        currentSourcePositionF.x = p_sourcesPositions[projectionIndex].x / p_volumeVoxelsSpacing.x;
        currentSourcePositionF.y = p_sourcesPositions[projectionIndex].y / p_volumeVoxelsSpacing.y;
        currentSourcePositionF.z = p_sourcesPositions[projectionIndex].z / p_volumeVoxelsSpacing.z;

        float3 currentProjectionsOriginF;
        currentProjectionsOriginF.x = p_projectionsOriginInWorld[projectionIndex].x / p_volumeVoxelsSpacing.x;
        currentProjectionsOriginF.y = p_projectionsOriginInWorld[projectionIndex].y / p_volumeVoxelsSpacing.y;
        currentProjectionsOriginF.z = p_projectionsOriginInWorld[projectionIndex].z / p_volumeVoxelsSpacing.z;

        float3 currentProjectionsEndF;
        currentProjectionsEndF.x = ( p_projectionsOriginInWorld[projectionIndex].x + static_cast<float>( p_projectionsDimension.x ) * p_projectionsPixelSpacing.x ) / p_volumeVoxelsSpacing.x;
        currentProjectionsEndF.y = ( p_projectionsOriginInWorld[projectionIndex].y + static_cast<float>( p_projectionsDimension.y ) * p_projectionsPixelSpacing.y ) / p_volumeVoxelsSpacing.y;
        currentProjectionsEndF.z = currentProjectionsOriginF.z;
         

        const auto pixelNeighborhoodSemiLength{ 5 }; // When the detected projection position is given by position (x,y), we look in a neighborhood of size 2*pixelNeighborhoodSemiLength+1
        bool constainsAtLeatOneProjectionPixel{ false };

        float3 currentProjectionPositionF; 
        currentProjectionPositionF.x = currentVoxelF.x + 0.5F;
        currentProjectionPositionF.y = currentVoxelF.y + 0.5F;
        currentProjectionPositionF.z = currentVoxelF.z + 0.5F;
        float3 currentProjectionRayDirectorVectorF;
        currentProjectionRayDirectorVectorF.x = currentProjectionPositionF.x - currentSourcePositionF.x;
        currentProjectionRayDirectorVectorF.y = currentProjectionPositionF.y - currentSourcePositionF.y;
        currentProjectionRayDirectorVectorF.z = currentProjectionPositionF.z - currentSourcePositionF.z;

        const auto alphaZ = fabs( currentProjectionRayDirectorVectorF.z ) > floatTolerance ? ( currentProjectionsOriginF.z - currentSourcePositionF.z ) / currentProjectionRayDirectorVectorF.z : 0.F;
        float2 intersectionF;
        intersectionF.x = currentSourcePositionF.x + currentProjectionRayDirectorVectorF.x * alphaZ;
        intersectionF.y = currentSourcePositionF.y + currentProjectionRayDirectorVectorF.y * alphaZ;
        if( intersectionF.x > currentProjectionsOriginF.x - static_cast<float>( pixelNeighborhoodSemiLength ) * p_projectionsPixelSpacing.x / p_volumeVoxelsSpacing.x 
            && intersectionF.x < currentProjectionsEndF.x  + static_cast<float>( pixelNeighborhoodSemiLength ) * p_projectionsPixelSpacing.x / p_volumeVoxelsSpacing.x 
            && intersectionF.y > currentProjectionsOriginF.y - static_cast<float>( pixelNeighborhoodSemiLength ) * p_projectionsPixelSpacing.y / p_volumeVoxelsSpacing.y 
            && intersectionF.y < currentProjectionsEndF.y + static_cast<float>( pixelNeighborhoodSemiLength ) * p_projectionsPixelSpacing.y / p_volumeVoxelsSpacing.y )    // we are on the detector :)
        { 
             constainsAtLeatOneProjectionPixel = true;
        }
        //         
        //    
        //        // For the eight corners of a voxel, we find the min and max in each dimension of the involved pixels on projection
//        float3 currentVoxelcurrentCornerF;
//        float3 currentRayDirectorVectorForCornerF;
//
//        float2 minProjectionPositionF;
//        minProjectionPositionF.x = currentProjectionsEndF.x + 1.F;
//        minProjectionPositionF.y = currentProjectionsEndF.y + 1.F;
//        float2 maxProjectionPositionF;
//        maxProjectionPositionF.x = currentProjectionsOriginF.x - 1.F;
//        maxProjectionPositionF.y = currentProjectionsOriginF.y - 1.F;
//        bool constainsAtLeatOneProjectionPixel{ false };
//        for( auto lagZ{ 0 }; lagZ <= 1; lagZ++ )
//        {
//            currentVoxelcurrentCornerF.z = currentVoxelF.z + static_cast<float>( lagZ ) - 0.5F;
//            currentRayDirectorVectorForCornerF.z = currentVoxelcurrentCornerF.z - currentSourcePositionF.z;
//            const auto alphaZ = fabs( currentRayDirectorVectorForCornerF.z ) > floatTolerance ? ( currentProjectionsOriginF.z - currentSourcePositionF.z ) / currentRayDirectorVectorForCornerF.z : 0.F;
//            float2 intersectionF;
//            for( auto lagX{ 0 }; lagX <= 1; lagX++ )
//            {
//                currentVoxelcurrentCornerF.x = currentVoxelF.x + static_cast<float>( lagX ) - 0.5F;
//                currentRayDirectorVectorForCornerF.x = currentVoxelcurrentCornerF.x - currentSourcePositionF.x;
//                intersectionF.x = currentSourcePositionF.x + currentRayDirectorVectorForCornerF.x * alphaZ;
//                for( auto lagY{ 0 }; lagY <= 1; lagY++ )
//                {
//                    currentVoxelcurrentCornerF.y = currentVoxelF.y + static_cast<float>( lagY ) - 0.5F;
//                    currentRayDirectorVectorForCornerF.y = currentVoxelcurrentCornerF.y - currentSourcePositionF.y;
//                    intersectionF.y = currentSourcePositionF.y + currentRayDirectorVectorForCornerF.y * alphaZ;
//
////#ifdef DEBUGPBACKROJECTION
////                    if( debuggingIndex == volumeIndex )
////                    {
////                        printf( "\t\t\t\t possible intersection PositionF ( %f , %f )\n",
////                                intersectionF.x,
////                                intersectionF.y );
////                    }
////#endif
//
//                    if( floatSupEq( intersectionF.x, currentProjectionsOriginF.x, floatTolerance )
//                        && intersectionF.x < currentProjectionsEndF.x
//                        && floatSupEq( intersectionF.y, currentProjectionsOriginF.y, floatTolerance )
//                        && intersectionF.y < currentProjectionsEndF.y )    // we are on the detector :)
//                    { 
//                        constainsAtLeatOneProjectionPixel = true;
//                        if( intersectionF.x < minProjectionPositionF.x )
//                        {
//                            minProjectionPositionF.x = intersectionF.x;
//                        }
//                        if( intersectionF.x > maxProjectionPositionF.x )
//                        {
//                            maxProjectionPositionF.x = intersectionF.x;
//                        }
//                        if( intersectionF.y < minProjectionPositionF.y )
//                        {
//                            minProjectionPositionF.y = intersectionF.y;
//                        }
//                        if( intersectionF.y > maxProjectionPositionF.y )
//                        {
//                            maxProjectionPositionF.y = intersectionF.y;
//                        }
//                    } 
//                }
//            }
//        }
//
//#ifdef DEBUGPBACKROJECTION
//        if( debuggingIndex == volumeIndex )
//        {
//            if( constainsAtLeatOneProjectionPixel )
//            {
//                printf( " \t\t we are inside detector \n" );
//            }
//            else
//            {
//                printf( " \t\t we are is outside detector \n" );
//            }
//            printf( " \t\t minProjectionPositionF ( %f , %f ) \n \t\t maxProjectionPositionF ( %f , %f ) \n",
//                    minProjectionPositionF.x,
//                    minProjectionPositionF.y,
//                    maxProjectionPositionF.x,
//                    maxProjectionPositionF.y );
//        }
//#endif

        if( constainsAtLeatOneProjectionPixel )
        { 
            //// min and max pixel computation:
            //// same process : by multiplying min(max)ProjectionPositionF by p_volumeVoxelsSpacing, we reset in World coordinates
            //// afterwards, by dividing by p_projectionsPixelSpacing (and casting in int), we can retrieve the pixel index
            //auto infProjectionPixelX = static_cast<int>( ( minProjectionPositionF.x  - currentProjectionsOriginF.x ) * p_volumeVoxelsSpacing.x / p_projectionsPixelSpacing.x );
            //infProjectionPixelX = infProjectionPixelX < 0 ? 0 : infProjectionPixelX >= p_projectionsDimension.x ? p_projectionsDimension.x - 1
            //                                                                                                    : infProjectionPixelX;    // clamping
            //auto infProjectionPixelY = static_cast<int>( ( minProjectionPositionF.y   - currentProjectionsOriginF.y ) * p_volumeVoxelsSpacing.y / p_projectionsPixelSpacing.y );
            //infProjectionPixelY = infProjectionPixelY < 0 ? 0 : infProjectionPixelY >= p_projectionsDimension.y ? p_projectionsDimension.y - 1
            //                                                                                                    : infProjectionPixelY;    // clamping
            //auto supProjectionPixelX = static_cast<int>( ( maxProjectionPositionF.x - currentProjectionsOriginF.x ) * p_volumeVoxelsSpacing.x / p_projectionsPixelSpacing.x );
            //supProjectionPixelX = supProjectionPixelX < 0 ? 0 : supProjectionPixelX >= p_projectionsDimension.x ? p_projectionsDimension.x - 1
            //                                                                                                    : supProjectionPixelX;    // clamping
            //auto supProjectionPixelY = static_cast<int>( ( maxProjectionPositionF.y  - currentProjectionsOriginF.y ) * p_volumeVoxelsSpacing.y / p_projectionsPixelSpacing.y );
            //supProjectionPixelY = supProjectionPixelY < 0 ? 0 : supProjectionPixelY >= p_projectionsDimension.y ? p_projectionsDimension.y - 1
            //                                                                                                    : supProjectionPixelY;    // clamping
//
//#ifdef DEBUGPBACKROJECTION
//            if( debuggingIndex == volumeIndex )
//            {
//                printf( " \t\t infProjectionPixel ( %d , %d ) \n \t\t supProjectionPixel ( %d , %d ) \n",
//                        infProjectionPixelX,
//                        infProjectionPixelY,
//                        supProjectionPixelX,
//                        supProjectionPixelY );
//            }
//#endif
             
            const auto pixelX = static_cast<int>( ( intersectionF.x - currentProjectionsOriginF.x ) * p_volumeVoxelsSpacing.x / p_projectionsPixelSpacing.x );
            const auto pixelY = static_cast<int>( ( intersectionF.y - currentProjectionsOriginF.y ) * p_volumeVoxelsSpacing.y / p_projectionsPixelSpacing.y );
            auto infProjectionPixelX = pixelX - pixelNeighborhoodSemiLength;
            if( infProjectionPixelX < 0 )
            {
                infProjectionPixelX = 0;
            }
            else if( infProjectionPixelX >= p_projectionsDimension.x )
            {
                infProjectionPixelX = p_projectionsDimension.x - 1;
            }
            
            auto supProjectionPixelX = pixelX + pixelNeighborhoodSemiLength;
            if( supProjectionPixelX < 0 )
            {
                supProjectionPixelX = 0;
            }
            else if( supProjectionPixelX >= p_projectionsDimension.x )
            {
                supProjectionPixelX = p_projectionsDimension.x - 1;
            }

            auto infProjectionPixelY = pixelY - pixelNeighborhoodSemiLength;
            if( infProjectionPixelY < 0 )
            {
                infProjectionPixelY = 0;
            }
            else if( infProjectionPixelY >= p_projectionsDimension.y )
            {
                infProjectionPixelY = p_projectionsDimension.y - 1;
            }
            auto supProjectionPixelY = pixelY + pixelNeighborhoodSemiLength;
            if( supProjectionPixelY < 0 )
            {
                supProjectionPixelY = 0;
            }
            else if( supProjectionPixelY >= p_projectionsDimension.y )
            {
                supProjectionPixelY = p_projectionsDimension.y - 1;
            }
            float3 alpha1;
            float3 alpha2;
            const auto currentIntersectionVoxelZlagIndex = projectionIndex * p_projectionsDimension.x * p_projectionsDimension.y;
            // Now we go through the invovled pixels
            currentProjectionPositionF.z = p_projectionsOriginInWorld[projectionIndex].z / p_volumeVoxelsSpacing.z;
            currentProjectionRayDirectorVectorF.z = currentProjectionPositionF.z - currentSourcePositionF.z;
            alpha1.z = fabs( currentProjectionRayDirectorVectorF.z ) > floatTolerance ? ( currentVoxelF.z - currentSourcePositionF.z ) / currentProjectionRayDirectorVectorF.z : 0.F;
            alpha2.z = fabs( currentProjectionRayDirectorVectorF.z ) > floatTolerance ? ( currentVoxelF.z + 1.F - currentSourcePositionF.z ) / currentProjectionRayDirectorVectorF.z : 0.F;

            for( auto projectionPixelY{ infProjectionPixelY }; projectionPixelY <= supProjectionPixelY; projectionPixelY++ )
            {
                const auto currentIntersectionVoxelYlagIndex = currentIntersectionVoxelZlagIndex + projectionPixelY * p_projectionsDimension.x ;
                currentProjectionPositionF.y = ( p_projectionsOriginInWorld[projectionIndex].y + ( static_cast<float>( projectionPixelY ) + 0.5F ) * p_projectionsPixelSpacing.y ) / p_volumeVoxelsSpacing.y;
                currentProjectionRayDirectorVectorF.y = currentProjectionPositionF.y - currentSourcePositionF.y;
                alpha1.y = fabs( currentProjectionRayDirectorVectorF.y ) > floatTolerance ? ( currentVoxelF.y - currentSourcePositionF.y ) / currentProjectionRayDirectorVectorF.y : 0.F;
                alpha2.y = fabs( currentProjectionRayDirectorVectorF.y ) > floatTolerance ? ( currentVoxelF.y + 1.F - currentSourcePositionF.y ) / currentProjectionRayDirectorVectorF.y : 0.F;

                for( auto projectionPixelX{ infProjectionPixelX }; projectionPixelX <= supProjectionPixelX; projectionPixelX++ )
                {
                    currentProjectionPositionF.x = ( p_projectionsOriginInWorld[projectionIndex].x + ( static_cast<float>( projectionPixelX ) + 0.5F ) * p_projectionsPixelSpacing.x ) / p_volumeVoxelsSpacing.x;
                    currentProjectionRayDirectorVectorF.x = currentProjectionPositionF.x - currentSourcePositionF.x;
                    alpha1.x = fabs( currentProjectionRayDirectorVectorF.x ) > floatTolerance ? ( currentVoxelF.x - currentSourcePositionF.x ) / currentProjectionRayDirectorVectorF.x : 0.F;
                    alpha2.x = fabs( currentProjectionRayDirectorVectorF.x ) > floatTolerance ? ( currentVoxelF.x + 1.F - currentSourcePositionF.x ) / currentProjectionRayDirectorVectorF.x : 0.F;

//                    
//#ifdef DEBUGPBACKROJECTION
//                    if( debuggingIndex == volumeIndex  )
//                    { 
//                        printf( " \t\t projectionPixel ( %d , %d ) of detector %d ",
//                                projectionPixelX,
//                                projectionPixelY,
//                                projectionIndex /*, 
//                                currentProjectionPositionF.x,
//                                currentProjectionPositionF.y,
//                                currentProjectionPositionF.z,
//                                currentProjectionRayDirectorVectorF.x,
//                                currentProjectionRayDirectorVectorF.y,
//                                currentProjectionRayDirectorVectorF.z,
//                                alpha1.x,
//                                alpha1.y,
//                                alpha1.z, 
//                                alpha2.x,
//                                alpha2.y,
//                                alpha2.z */
//                        );
//                    }
//#endif 

                    float3 alphaMins;    // alpha corresponding to the bottom left front vertex
                    alphaMins.x = fmin( alpha1.x, alpha2.x );
                    alphaMins.y = fmin( alpha1.y, alpha2.y );
                    alphaMins.z = fmin( alpha1.z, alpha2.z );
                    float3 alphaMaxs;    // alpha corresponding to the top right back vertex
                    alphaMaxs.x = fmax( alpha1.x, alpha2.x );
                    alphaMaxs.y = fmax( alpha1.y, alpha2.y );
                    alphaMaxs.z = fmax( alpha1.z, alpha2.z );


                    // We take the highest of the mins to catch the incoming intersection alpha
                    auto alphaMin = fmax( fmax( 0.F, alphaMins.x ), fmax( alphaMins.y, alphaMins.z ) );
                    // We take the lowest of the maxs to catch the outgoing intersection alpha
                    auto alphaMax = fmin( fmin( 1.F, alphaMaxs.x ), fmin( alphaMaxs.y, alphaMaxs.z ) );
                    // alphaMin and alphaMax constructions above guaranties the positiveness of 
                    if( floatSupEq( alphaMax, alphaMin ) )
                    {
                        const auto weight = ( alphaMax - alphaMin );

#ifdef DEBUGPBACKROJECTION
                        if( debuggingIndex == volumeIndex )
                        {
                            printf( "\t\t projectionPixel( %d, %d ) of detector %d (whose value is %f) has weight %f \n", 
                                    projectionPixelX,
                                    projectionPixelY,
                                    projectionIndex,
                                    p_projectionsBuffer[currentIntersectionVoxelYlagIndex + projectionPixelX],
                                    weight );
                        }
#endif 
                        if( weight > floatTolerance )
                        {
                            totalWeight += weight;
                            total += weight * p_projectionsBuffer[currentIntersectionVoxelYlagIndex + projectionPixelX];
                        }
                    }
#ifdef DEBUGPBACKROJECTION
                    //else
                    //{
                    //    if( debuggingIndex == volumeIndex   )
                    //    {
                    //        printf( " has no weight \n" );
                    //    }
                    //}
 
                    // //( 404 , 453 ) of detector 37 ( 405 , 452 ) of detector 37
                    //if( debuggingIndex == volumeIndex && projectionIndex == 37 )
                    //{  
                    //    if( ( projectionPixelX == 404 && projectionPixelY == 453 ) || ( projectionPixelX == 405 && projectionPixelY == 452 ) )
                    //        printf( " \t\t currentProjectionPositionF ( %f , %f , %f ) \n \t\t currentProjectionRayDirectorVectorF ( %f , %f , %f )\n \t\t alpha1 ( %f , %f , %f )\n \t\t alpha2 ( %f , %f , %f )\n \t\t alphaMins ( %f , %f , %f )\n \t\t alphaMaxs ( %f , %f , %f )\n \t\t alphaMin = %f  alphaMax = %f\n", 
                    //            currentProjectionPositionF.x,
                    //            currentProjectionPositionF.y,
                    //            currentProjectionPositionF.z,
                    //            currentProjectionRayDirectorVectorF.x,
                    //            currentProjectionRayDirectorVectorF.y,
                    //            currentProjectionRayDirectorVectorF.z,
                    //            alpha1.x,
                    //            alpha1.y,
                    //            alpha1.z, 
                    //            alpha2.x,
                    //            alpha2.y,
                    //            alpha2.z,
                    //            alphaMins.x,
                    //            alphaMins.y,
                    //            alphaMins.z,
                    //            alphaMaxs.x,
                    //            alphaMaxs.y,
                    //            alphaMaxs.z,
                    //            alphaMin,
                    //            alphaMax
                    //    );
                    //}
#endif
                }
            }
        } 
    }

    if( totalWeight > floatTolerance )
    {
        p_volumeBuffer[volumeIndex] = total / totalWeight;
    }

#ifdef DEBUGPBACKROJECTION
    if( debuggingIndex == volumeIndex )
    {
    printf( " \t\t result : %f \n ",
                p_volumeBuffer[volumeIndex] );
    }
#endif
}
void Projector::ExtractDimension( const vtkSmartPointer<vtkImageData> p_imageDataPtr, dim3 & p_dimension ) const
{
    if (p_imageDataPtr == nullptr)
    {
        return;
    }
    int dimensions[3];
    p_imageDataPtr->GetDimensions( dimensions ); 
    p_dimension.x = dimensions[0];
    p_dimension.y = dimensions[1];
    p_dimension.z = dimensions[2];
}
void Projector::ExtractSpacing( const vtkSmartPointer<vtkImageData> p_imageDataPtr, float3 & p_spacing ) const
{
    if( p_imageDataPtr == nullptr )
    {
        return;
    }
    double spacing[3];
    p_imageDataPtr->GetSpacing( spacing );
    p_spacing.x = static_cast<float>( spacing[0] );
    p_spacing.y = static_cast<float>( spacing[1] );
    p_spacing.z = static_cast<float>( spacing[2] );
}
void Projector::ExtractOrigin( const vtkSmartPointer<vtkImageData> p_imageDataPtr, float3 & p_origin ) const
{
    if( p_imageDataPtr == nullptr )
    {
        return;
    }
    double origin[3];
    p_imageDataPtr->GetOrigin( origin );
    p_origin.x = static_cast<float>( origin[0] );
    p_origin.y = static_cast<float>( origin[1] );
    p_origin.z = static_cast<float>( origin[2] );
}

void Projector::PerformProjection( const vtkSmartPointer<vtkImageData> p_volume, const Position3D & p_sourcePosition, vtkSmartPointer<vtkImageData> p_projectionsContainer ) const
{
    //// Device memory for volume elements 
    dim3 volumeDimensions; 
    this->ExtractDimension( p_volume, volumeDimensions );
    float3 volumeVoxelsSpacing;
    this->ExtractSpacing( p_volume, volumeVoxelsSpacing );
    float3 volumeOrigin;
    this->ExtractOrigin( p_volume, volumeOrigin );

    // declaration and preparation of buffer (for device)
    float * d_volumeBuffer;
    const auto volumeDimSize = volumeDimensions.z * volumeDimensions.y * volumeDimensions.x;
    unsigned int memSizeVolumeBuffer = volumeDimSize * sizeof( float );
    checkCudaErrors( cudaMalloc( (void **)&d_volumeBuffer, memSizeVolumeBuffer ) );
    // copy to device memory
    const auto * volumeBuffer = static_cast<float *>( p_volume->GetScalarPointer() );
    checkCudaErrors( cudaMemcpy( d_volumeBuffer, volumeBuffer, memSizeVolumeBuffer, cudaMemcpyHostToDevice ) );

    //// Device memory for projections elements (result)
    dim3 projectionDimensions;
    this->ExtractDimension( p_projectionsContainer, projectionDimensions ); 
    
    float3 tempoProjectionsPixelSpacing;
    this->ExtractSpacing( p_projectionsContainer, tempoProjectionsPixelSpacing ); 
    float2 projectionsPixelSpacing;
    projectionsPixelSpacing.x = tempoProjectionsPixelSpacing.x;
    projectionsPixelSpacing.y = tempoProjectionsPixelSpacing.y;

    float * d_projectionBuffer;
    const auto projectionDimSize = projectionDimensions.z * projectionDimensions.y * projectionDimensions.x;
    unsigned int memSizeProjectionBuffer = projectionDimSize * sizeof( float );
    checkCudaErrors( cudaMalloc( (void **)&d_projectionBuffer, memSizeProjectionBuffer ) );


    float3 * projectionsOriginInWorld = new float3[projectionDimensions.z];
    for( auto projectionIndex{ 0 }; projectionIndex < projectionDimensions.z; projectionIndex++ )
    {
        projectionsOriginInWorld[projectionIndex].x = m_tomoGeometry->projectionsRoisBottomLeftPositions().at( projectionIndex ).x;
        projectionsOriginInWorld[projectionIndex].y = m_tomoGeometry->projectionsRoisBottomLeftPositions().at( projectionIndex ).y;
        projectionsOriginInWorld[projectionIndex].z = m_tomoGeometry->detectorsZCommonPosition();
    }
    unsigned int memSizeProjectionOrigins = projectionDimensions.z * sizeof( float3 );
    float3 * d_projectionsOriginInWorld;
    checkCudaErrors( cudaMalloc( (void **)&d_projectionsOriginInWorld, memSizeProjectionOrigins ) );
    checkCudaErrors( cudaMemcpy( d_projectionsOriginInWorld, projectionsOriginInWorld, memSizeProjectionOrigins, cudaMemcpyHostToDevice ) );

}


vtkSmartPointer<vtkImageData>  Projector::PerformProjection( vtkSmartPointer<vtkImageData>  p_volume ) const
{
    //// Device memory for volume elements
    dim3 volumeDimensions;
    volumeDimensions.x = m_tomoGeometry->GetVolume()->GetSize3D().x;
    volumeDimensions.y = m_tomoGeometry->GetVolume()->GetSize3D().y;
    volumeDimensions.z = m_tomoGeometry->GetVolume()->GetSize3D().z;

    float3 volumeOrigin;
    volumeOrigin.x = m_tomoGeometry->GetVolume()->GetBLF().x;
    volumeOrigin.y = m_tomoGeometry->GetVolume()->GetBLF().y;
    volumeOrigin.z = m_tomoGeometry->GetVolume()->GetBLF().z;

    float3 volumeVoxelsSpacing;
    volumeVoxelsSpacing.x = m_tomoGeometry->GetVolume()->GetVoxelSpacing().x;
    volumeVoxelsSpacing.y = m_tomoGeometry->GetVolume()->GetVoxelSpacing().y;
    volumeVoxelsSpacing.z = m_tomoGeometry->GetVolume()->GetVoxelSpacing().z;


    // declaration and preparation of buffer (for device)
    float * d_volumeBuffer;
    const auto volumeDimSize = volumeDimensions.z * volumeDimensions.y * volumeDimensions.x;
    unsigned int memSizeVolumeBuffer = volumeDimSize * sizeof( float );
    checkCudaErrors( cudaMalloc( (void **)&d_volumeBuffer, memSizeVolumeBuffer ) );
    // copy to device memory
    const auto * volumeBuffer = static_cast<float *>( p_volume->GetScalarPointer() );
    checkCudaErrors( cudaMemcpy( d_volumeBuffer, volumeBuffer, memSizeVolumeBuffer, cudaMemcpyHostToDevice ) );

    //// Device memory for projections elements (result)
    dim3 projectionDimensions;
    projectionDimensions.z = m_tomoGeometry->nbProjectionsRois();
    projectionDimensions.x = m_tomoGeometry->projectionsRoisSize().x;
    projectionDimensions.y = m_tomoGeometry->projectionsRoisSize().y;

    float2 projectionsPixelSpacing;
    projectionsPixelSpacing.x = m_tomoGeometry->projectionsPixelSpacing().x;
    projectionsPixelSpacing.y = m_tomoGeometry->projectionsPixelSpacing().y;

    float * d_projectionBuffer;
    const auto projectionDimSize = projectionDimensions.z * projectionDimensions.y * projectionDimensions.x;
    unsigned int memSizeProjectionBuffer = projectionDimSize * sizeof( float );
    checkCudaErrors( cudaMalloc( (void **)&d_projectionBuffer, memSizeProjectionBuffer ) );


    float3 * projectionsOriginInWorld = new float3[projectionDimensions.z];
    for( auto projectionIndex{ 0 }; projectionIndex < projectionDimensions.z; projectionIndex++ )
    {
        projectionsOriginInWorld[projectionIndex].x = m_tomoGeometry->projectionsRoisBottomLeftPositions().at( projectionIndex ).x;
        projectionsOriginInWorld[projectionIndex].y = m_tomoGeometry->projectionsRoisBottomLeftPositions().at( projectionIndex ).y;
        projectionsOriginInWorld[projectionIndex].z = m_tomoGeometry->detectorsZCommonPosition();
    }
    unsigned int memSizeProjectionOrigins = projectionDimensions.z * sizeof( float3 );
    float3 * d_projectionsOriginInWorld;
    checkCudaErrors( cudaMalloc( (void **)&d_projectionsOriginInWorld, memSizeProjectionOrigins ) );
    checkCudaErrors( cudaMemcpy( d_projectionsOriginInWorld, projectionsOriginInWorld, memSizeProjectionOrigins, cudaMemcpyHostToDevice ) );

    //// Device memory for sources elements
    float3 * sourcePositionsInWorld = new float3[projectionDimensions.z];
    for( auto projectionIndex{ 0 }; projectionIndex < projectionDimensions.z; projectionIndex++ )
    {
        sourcePositionsInWorld[projectionIndex].x = m_tomoGeometry->sourcesXCommonPosition();
        sourcePositionsInWorld[projectionIndex].y = m_tomoGeometry->sourcesYPositions().at( projectionIndex );
        sourcePositionsInWorld[projectionIndex].z = m_tomoGeometry->sourcesZCommonPosition();
    }
    unsigned int memSizeSourcesPositions = projectionDimensions.z * sizeof( float3 );
    float3 * d_sourcePositionsInWorld;
    checkCudaErrors( cudaMalloc( (void **)&d_sourcePositionsInWorld, memSizeSourcesPositions ) );
    checkCudaErrors( cudaMemcpy( d_sourcePositionsInWorld, sourcePositionsInWorld, memSizeSourcesPositions, cudaMemcpyHostToDevice ) );

    dim3 blockDims( 32, 16, 1 );
    dim3 gridDims( static_cast<unsigned int>( ceil( static_cast<double>( projectionDimensions.x ) / static_cast<double>( blockDims.x ) ) ),
                   static_cast<unsigned int>( ceil( static_cast<double>( projectionDimensions.y ) / static_cast<double>( blockDims.y ) ) ),
                   static_cast<unsigned int>( ceil( static_cast<double>( projectionDimensions.z ) / static_cast<double>( blockDims.z ) ) ) );

    //dim3 gridDims( 1,1,1 );
    std::cout << "projectionDimSize " << projectionDimSize << std::endl;
    std::cout << "projectionDimensions " << projectionDimensions.x << " , " << projectionDimensions.y << " , " << projectionDimensions.z << std::endl;
    std::cout << "blockDims " << blockDims.x << " , " << blockDims.y << " , " << blockDims.z << std::endl;
    std::cout << "gridDims " << gridDims.x << " , " << gridDims.y << " , " << gridDims.z << std::endl;


    CudaPerformProjection<<<gridDims, blockDims>>>( d_volumeBuffer, volumeDimensions, volumeOrigin, volumeVoxelsSpacing, d_projectionBuffer, projectionDimensions, projectionsPixelSpacing, d_projectionsOriginInWorld, d_sourcePositionsInWorld );

    // Wait for GPU to finish before accessing on host
    cudaDeviceSynchronize();

    auto errorCode = cudaGetLastError();
    std::cout << errorCode << std::endl;
    std::cout << cudaGetErrorName( errorCode ) << std::endl;
    std::cout << cudaGetErrorString( errorCode ) << std::endl;

    // Prepare data for output
    auto outputImage = vtkSmartPointer<vtkImageData> ::New();
    outputImage->SetDimensions( projectionDimensions.x, projectionDimensions.y, projectionDimensions.z );
    outputImage->SetSpacing( m_tomoGeometry->projectionsPixelSpacing().x, m_tomoGeometry->projectionsPixelSpacing().y, 1. );
    outputImage->AllocateScalars( VTK_FLOAT, 1 );
    auto finalImageBuffer = static_cast<float *>( outputImage->GetScalarPointer() );

    errorCode = cudaMemcpy( finalImageBuffer, d_projectionBuffer, memSizeProjectionBuffer, cudaMemcpyDeviceToHost );
    std::cout << errorCode << std::endl;
    std::cout << cudaGetErrorName( errorCode ) << std::endl;
    std::cout << cudaGetErrorString( errorCode ) << std::endl;

    auto tiffWriterOutput = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriterOutput->SetFileName( "../Output.tiff" );
    tiffWriterOutput->SetInputData( outputImage );
    tiffWriterOutput->Write();

    checkCudaErrors( cudaFree( d_sourcePositionsInWorld ) );
    free( sourcePositionsInWorld );
    checkCudaErrors( cudaFree( d_projectionsOriginInWorld ) );
    free( projectionsOriginInWorld );
    checkCudaErrors( cudaFree( d_projectionBuffer ) );
    checkCudaErrors( cudaFree( d_volumeBuffer ) );

    return outputImage;

    ////double flopsPerMatrixMul = 2.0 * static_cast<double>( dimsA.x ) * static_cast<double>( dimsA.y ) * static_cast<double>( dimsB.x );
    ////double gigaFlops =
    ////  ( flopsPerMatrixMul * 1.0e-9f ) / ( msecPerMatrixMul / 1000.0f );
    ////printf(
    ////  "Performance= %.2f GFlop/s, Time= %.3f msec, Size= %.0f Ops,"
    ////  " WorkgroupSize= %u threads/block\n",
    ////  gigaFlops,
    ////  msecPerMatrixMul,
    ////  flopsPerMatrixMul,
    ////  threads.x * threads.y );
    //
    ////printf( "Checking computed result for correctness: " );
    ////bool correct = true;

    ////// test relative error by the formula
    //////     |<x, y>_cpu - <x,y>_gpu|/<|x|, |y|>  < eps
    ////double eps = 1.e-6;    // machine zero

    ////for( int i = 0; i < static_cast<int>( dimsC.x * dimsC.y ); i++ )
    ////{
    ////    double abs_err = fabs( h_C[i] - ( dimsA.x * valB ) );
    ////    double dot_length = dimsA.x;
    ////    double abs_val = fabs( h_C[i] );
    ////    double rel_err = abs_err / abs_val / dot_length;

    ////    if( rel_err > eps )
    ////    {
    ////        printf( "Error! Matrix[%05d]=%.8f, ref=%.8f error term is > %E\n", i, h_C[i], dimsA.x * valB, eps );
    ////        correct = false;
    ////    }
    ////}
    ////
    ////printf( "%s\n", correct ? "Result = PASS" : "Result = FAIL" );
    //
    ////printf(
    ////  "\nNOTE: The CUDA Samples are not meant for performance"
    ////  "measurements. Results may vary when GPU Boost is enabled.\n" );

    ////if( correct )
    ////{
    ////    return EXIT_SUCCESS;
    ////}
    ////else
    ////{
    ////    return EXIT_FAILURE;
    ////}
}

vtkSmartPointer<vtkImageData>  Projector::PerformBackProjection( vtkSmartPointer<vtkImageData>  p_projections ) const
{
    //// Device memory for volume elements (result)
    dim3 volumeDimensions;
    volumeDimensions.x = m_tomoGeometry->GetVolume()->GetSize3D().x;
    volumeDimensions.y = m_tomoGeometry->GetVolume()->GetSize3D().y;
    volumeDimensions.z = m_tomoGeometry->GetVolume()->GetSize3D().z;

    float3 volumeOrigin;
    volumeOrigin.x = m_tomoGeometry->GetVolume()->GetBLF().x;
    volumeOrigin.y = m_tomoGeometry->GetVolume()->GetBLF().y;
    volumeOrigin.z = m_tomoGeometry->GetVolume()->GetBLF().z;

    float3 volumeVoxelsSpacing;
    volumeVoxelsSpacing.x = m_tomoGeometry->GetVolume()->GetVoxelSpacing().x;
    volumeVoxelsSpacing.y = m_tomoGeometry->GetVolume()->GetVoxelSpacing().y;
    volumeVoxelsSpacing.z = m_tomoGeometry->GetVolume()->GetVoxelSpacing().z;

    float * d_volumeBuffer;
    const auto volumeDimSize = volumeDimensions.z * volumeDimensions.y * volumeDimensions.x;
    const auto memSizeVolumeBuffer = volumeDimSize * static_cast<unsigned int>( sizeof( float ) );
    checkCudaErrors( cudaMalloc( (void **)&d_volumeBuffer, memSizeVolumeBuffer ) );


    //// Device memory for projections elements
    dim3 projectionDimensions;
    projectionDimensions.z = m_tomoGeometry->nbProjectionsRois();
    projectionDimensions.x = m_tomoGeometry->projectionsRoisSize().x;
    projectionDimensions.y = m_tomoGeometry->projectionsRoisSize().y;

    // declaration and preparation
    float * d_projectionBuffer;
    const auto projectionDimSize = projectionDimensions.z * projectionDimensions.y * projectionDimensions.x;
    unsigned int memSizeProjectionBuffer = projectionDimSize * sizeof( float );
    checkCudaErrors( cudaMalloc( (void **)&d_projectionBuffer, memSizeProjectionBuffer ) );
    // copy to device memory
    auto projectionBuffer = static_cast<float *>( p_projections->GetScalarPointer() );
    checkCudaErrors( cudaMemcpy( d_projectionBuffer, projectionBuffer, memSizeProjectionBuffer, cudaMemcpyHostToDevice ) );


    float2 projectionsPixelsSpacing;
    projectionsPixelsSpacing.x = m_tomoGeometry->projectionsPixelSpacing().x;
    projectionsPixelsSpacing.y = m_tomoGeometry->projectionsPixelSpacing().y;

    float3 * projectionsOriginInWorld = new float3[projectionDimensions.z];
    for( auto projectionIndex{ 0 }; projectionIndex < projectionDimensions.z; projectionIndex++ )
    {
        projectionsOriginInWorld[projectionIndex].x = m_tomoGeometry->projectionsRoisBottomLeftPositions().at( projectionIndex ).x;
        projectionsOriginInWorld[projectionIndex].y = m_tomoGeometry->projectionsRoisBottomLeftPositions().at( projectionIndex ).y;
        projectionsOriginInWorld[projectionIndex].z = m_tomoGeometry->detectorsZCommonPosition();
    }
    unsigned int memSizeProjectionOrigins = projectionDimensions.z * sizeof( float3 );
    float3 * d_projectionsOriginInWorld;
    checkCudaErrors( cudaMalloc( (void **)&d_projectionsOriginInWorld, memSizeProjectionOrigins ) );
    checkCudaErrors( cudaMemcpy( d_projectionsOriginInWorld, projectionsOriginInWorld, memSizeProjectionOrigins, cudaMemcpyHostToDevice ) );

    //// Device memory for sources elements
    float3 * sourcePositionsInWorld = new float3[projectionDimensions.z];
    for( auto projectionIndex{ 0 }; projectionIndex < projectionDimensions.z; projectionIndex++ )
    {
        sourcePositionsInWorld[projectionIndex].x = m_tomoGeometry->sourcesXCommonPosition();
        sourcePositionsInWorld[projectionIndex].y = m_tomoGeometry->sourcesYPositions().at( projectionIndex );
        sourcePositionsInWorld[projectionIndex].z = m_tomoGeometry->sourcesZCommonPosition();
    }
    unsigned int memSizeSourcesPositions = projectionDimensions.z * sizeof( float3 );
    float3 * d_sourcePositionsInWorld;
    checkCudaErrors( cudaMalloc( (void **)&d_sourcePositionsInWorld, memSizeSourcesPositions ) );
    checkCudaErrors( cudaMemcpy( d_sourcePositionsInWorld, sourcePositionsInWorld, memSizeSourcesPositions, cudaMemcpyHostToDevice ) );

    dim3 blockDims( 32, 16, 1 );
    dim3 gridDims( static_cast<unsigned int>( ceil( static_cast<double>( volumeDimensions.x ) / static_cast<double>( blockDims.x ) ) ),
                   static_cast<unsigned int>( ceil( static_cast<double>( volumeDimensions.y ) / static_cast<double>( blockDims.y ) ) ),
                   static_cast<unsigned int>( ceil( static_cast<double>( volumeDimensions.z ) / static_cast<double>( blockDims.z ) ) ) );

    std::cout << "volumeDimensions " << volumeDimensions.x << " , " << volumeDimensions.y << " , " << volumeDimensions.z << std::endl;
    std::cout << "blockDims " << blockDims.x << " , " << blockDims.y << " , " << blockDims.z << std::endl;
    std::cout << "gridDims " << gridDims.x << " , " << gridDims.y << " , " << gridDims.z << std::endl;

    //const float * p_projectionsBuffer,
    //const dim3 p_volumeDimension,
    //const float3 p_volumeOriginInWorld,
    //const float3 p_volumeVoxelsSpacing,
    //float *p_volumeBuffer,
    //const dim3 p_projectionsDimension,
    //const float3 *p_projectionsOriginInWorld,
    //const float3 *p_sourcesPositions

    CudaPerformBackProjection<<<gridDims, blockDims>>>( d_projectionBuffer, volumeDimensions, volumeOrigin, volumeVoxelsSpacing, d_volumeBuffer, projectionDimensions, projectionsPixelsSpacing, d_projectionsOriginInWorld, d_sourcePositionsInWorld );

    // Wait for GPU to finish before accessing on host
    cudaDeviceSynchronize();

    auto errorCode = cudaGetLastError();
    std::cout << errorCode << std::endl;
    std::cout << cudaGetErrorName( errorCode ) << std::endl;
    std::cout << cudaGetErrorString( errorCode ) << std::endl;

    // Prepare data for output
    auto outputImage = vtkSmartPointer<vtkImageData> ::New();
    outputImage->SetDimensions( volumeDimensions.x, volumeDimensions.y, volumeDimensions.z );
    outputImage->SetSpacing( volumeVoxelsSpacing.x, volumeVoxelsSpacing.y, volumeVoxelsSpacing.z );
    outputImage->AllocateScalars( VTK_FLOAT, 1 );
    auto finalImageBuffer = static_cast<float *>( outputImage->GetScalarPointer() );

    errorCode = cudaMemcpy( finalImageBuffer, d_volumeBuffer, memSizeVolumeBuffer, cudaMemcpyDeviceToHost );
    std::cout << errorCode << std::endl;
    std::cout << cudaGetErrorName( errorCode ) << std::endl;
    std::cout << cudaGetErrorString( errorCode ) << std::endl;

    auto tiffWriterOutput = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriterOutput->SetFileName( "../Output.tiff" );
    tiffWriterOutput->SetInputData( outputImage );
    tiffWriterOutput->Write();

    checkCudaErrors( cudaFree( d_sourcePositionsInWorld ) );
    free( sourcePositionsInWorld );
    checkCudaErrors( cudaFree( d_projectionsOriginInWorld ) );
    free( projectionsOriginInWorld );
    checkCudaErrors( cudaFree( d_projectionBuffer ) );
    checkCudaErrors( cudaFree( d_volumeBuffer ) );

    return outputImage;

    ////double flopsPerMatrixMul = 2.0 * static_cast<double>( dimsA.x ) * static_cast<double>( dimsA.y ) * static_cast<double>( dimsB.x );
    ////double gigaFlops =
    ////  ( flopsPerMatrixMul * 1.0e-9f ) / ( msecPerMatrixMul / 1000.0f );
    ////printf(
    ////  "Performance= %.2f GFlop/s, Time= %.3f msec, Size= %.0f Ops,"
    ////  " WorkgroupSize= %u threads/block\n",
    ////  gigaFlops,
    ////  msecPerMatrixMul,
    ////  flopsPerMatrixMul,
    ////  threads.x * threads.y );
    //
    ////printf( "Checking computed result for correctness: " );
    ////bool correct = true;

    ////// test relative error by the formula
    //////     |<x, y>_cpu - <x,y>_gpu|/<|x|, |y|>  < eps
    ////double eps = 1.e-6;    // machine zero

    ////for( int i = 0; i < static_cast<int>( dimsC.x * dimsC.y ); i++ )
    ////{
    ////    double abs_err = fabs( h_C[i] - ( dimsA.x * valB ) );
    ////    double dot_length = dimsA.x;
    ////    double abs_val = fabs( h_C[i] );
    ////    double rel_err = abs_err / abs_val / dot_length;

    ////    if( rel_err > eps )
    ////    {
    ////        printf( "Error! Matrix[%05d]=%.8f, ref=%.8f error term is > %E\n", i, h_C[i], dimsA.x * valB, eps );
    ////        correct = false;
    ////    }
    ////}
    ////
    ////printf( "%s\n", correct ? "Result = PASS" : "Result = FAIL" );
    //
    ////printf(
    ////  "\nNOTE: The CUDA Samples are not meant for performance"
    ////  "measurements. Results may vary when GPU Boost is enabled.\n" );

    ////if( correct )
    ////{
    ////    return EXIT_SUCCESS;
    ////}
    ////else
    ////{
    ////    return EXIT_FAILURE;
    ////}
}


/*


__global__ void blur( float * input_image, float * output_image, int width, int height )
{
    const unsigned int offset = blockIdx.x * blockDim.x + threadIdx.x;
    int x = offset % width;
    int y = ( offset - x ) / width;
    int fsize = 5;    // Filter size
    if( offset < width * height )
    {
        float output_val = 0.F;
        int hits = 0;
        for( int ox = -fsize; ox < fsize + 1; ++ox )
        {
            for( int oy = -fsize; oy < fsize + 1; ++oy )
            {
                if( ( x + ox ) > -1 && ( x + ox ) < width && ( y + oy ) > -1 && ( y + oy ) < height )
                {
                    const int currentoffset = ( offset + ox + oy * width );
                    output_val += input_image[currentoffset];
                    hits++;
                }
            }
        }
        output_image[offset] = output_val / static_cast<float>( hits );
    }
}

void filter( float * input_image, float * output_image, int width, int height )
{
    float * dev_input;
    float * dev_output;
    checkCudaErrors( cudaMalloc( (void **)&dev_input, width * height *  sizeof( float ) ) );
    checkCudaErrors( cudaMemcpy( dev_input, input_image, width * height * sizeof( float ), cudaMemcpyHostToDevice ) );

    checkCudaErrors( cudaMalloc( (void **)&dev_output, width * height * sizeof( float ) ) );

    dim3 blockDims( 512, 1, 1 );
    dim3 gridDims( (unsigned int)ceil( (double)( width * height / blockDims.x ) ), 1, 1 );

    blur<<<gridDims, blockDims>>>( dev_input, dev_output, width, height );
     
    checkCudaErrors( cudaMemcpy( output_image, dev_output, width * height * sizeof( float ), cudaMemcpyDeviceToHost ) );

    checkCudaErrors( cudaFree( dev_input ) );
    checkCudaErrors( cudaFree( dev_output ) );
}


ImageDataPtr CreateFloatTestImage( const int p_width, const int p_height, const int p_depth )
{
    auto imageTest = ImageDataPtr::New();

    imageTest->SetExtent( 0, p_width - 1, 0, p_height - 1, 0, p_depth - 1 );
    imageTest->SetOrigin( 0.0, 0.0, 0.0 );
    imageTest->SetSpacing( 0.140, 0.140, 1.0 );    // set the same spacing as the Viework detector (because why not)
    imageTest->AllocateScalars( VTK_FLOAT, 1 );
     
    auto value = 0.F;
    for( int depth = 0; depth < p_depth; depth++ )
    {
        for( int row = 0; row < p_height; row++ )
        {
            for( int col = 0; col < p_width; col++ )
            {
                auto pixel = static_cast<float *>( imageTest->GetScalarPointer( col, row, depth ) );
                pixel[0] = value;
                value = value + 1.F;
            }
        }
    }
    return imageTest;
}
  
*/