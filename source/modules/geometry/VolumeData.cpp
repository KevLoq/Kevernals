#include "modules/geometry/VolumeData.h"

#include <cuda_runtime.h>
#include <numeric>

std::unique_ptr<VolumeData> VolumeData::Make( vtkSmartPointer<vtkImageData> p_volume )
{
    if( p_volume == nullptr )
    {
        return nullptr;
    }
    return std::unique_ptr<VolumeData>( new VolumeData( p_volume ) );
}

VolumeData::VolumeData( vtkSmartPointer<vtkImageData> p_volume )
{
    // Deep copy for avoiding any outside modification
    m_volume->DeepCopy( p_volume );
}

dim3 VolumeData::GetDimensionsCuda() const
{
    int dimensions[3];
    m_volume->GetDimensions( dimensions );
    dim3 dimensionsCuda;
    dimensionsCuda.x = dimensions[0];
    dimensionsCuda.y = dimensions[1];
    dimensionsCuda.z = dimensions[2];
    return dimensionsCuda;
}

float3 VolumeData::GetSpacingCuda() const
{
    double spacing[3];
    m_volume->GetSpacing( spacing );
    float3 spacingCuda;
    spacingCuda.x = static_cast<float>( spacing[0] );
    spacingCuda.y = static_cast<float>( spacing[1] );
    spacingCuda.z = static_cast<float>( spacing[2] );
    return spacingCuda;
}


float3 VolumeData::GetOriginCuda() const
{
    double origin[3];
    m_volume->GetOrigin( origin );
    float3 originCuda;
    originCuda.x = static_cast<float>( origin[0] );
    originCuda.y = static_cast<float>( origin[1] );
    originCuda.z = static_cast<float>( origin[2] );
    return originCuda;
}


std::vector<int> VolumeData::GetVoxelsIndices() const
{
    int dimensions[3];
    m_volume->GetDimensions( dimensions );
    const auto totalNumberOfVoxels = dimensions[0] * dimensions[1] * dimensions[2];
    std::vector<int> voxelsIndices( totalNumberOfVoxels );
    std::iota( voxelsIndices.begin(), voxelsIndices.end(), 0 );
    return voxelsIndices;
}

Position3D VolumeData::GetVoxelPosition( int p_col, int p_row, int p_depth )
{
    Position3D position;
    double origin[3];
    double spacing[3];
    m_volume->GetOrigin( origin );
    m_volume->GetSpacing( spacing );
    position.x = static_cast<float>( origin[0] + p_col * spacing[0] );
    position.y = static_cast<float>( origin[1] + p_row * spacing[1] );
    position.z = static_cast<float>( origin[2] + p_depth * spacing[2] );
    return position;
}

Position3D VolumeData::GetVoxelPosition( int p_voxelIndex )
{
    int dimensions[3];
    m_volume->GetDimensions( dimensions );
    const auto area = dimensions[0] * dimensions[1];
    const auto depth = p_voxelIndex / area;
    const auto indexOnImage = p_voxelIndex % area;
    const auto row = indexOnImage / dimensions[1];
    const auto column = indexOnImage % dimensions[1];
    return this->GetVoxelPosition( column, row, depth );
}