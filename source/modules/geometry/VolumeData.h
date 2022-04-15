#pragma once

#include "commons/GeometryUtils.h"

#include <vtkImageData.h>
#include <vtkSmartPointer.h>

#include <memory>
#include <vector>

struct dim3;
struct float3;

// Class encapsulating a deepCopy of vtkSmartPointer<vtkImageData> passed in Factory Make construction
// method.
// Aimed at facilitating the use of cuda structures
class VolumeData
{
public:
    static std::unique_ptr<VolumeData> Make( vtkSmartPointer<vtkImageData> p_volume );
    ~VolumeData() = default;

    dim3 GetDimensionsCuda() const;
    float3 GetOriginCuda() const;
    float3 GetSpacingCuda() const;

    std::vector<int> GetVoxelsIndices() const;

    Position3D GetVoxelPosition( int p_col, int p_row, int p_depth );
    Position3D GetVoxelPosition( int p_voxelIndex );


private:
    VolumeData( vtkSmartPointer<vtkImageData> p_volume );
    vtkSmartPointer<vtkImageData> m_volume;
};
