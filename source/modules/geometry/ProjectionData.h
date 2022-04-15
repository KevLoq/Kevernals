#pragma once

#include "commons/GeometryUtils.h"

#include <vtkImageData.h>
#include <vtkSmartPointer.h>

#include <cuda_runtime.h>
#include <memory>
#include <vector>


// Class encapsulating a deepCopy of vtkSmartPointer<vtkImageData> passed in Factory Make construction
// method.
// Aimed at facilitating the use of cuda structures
class ProjectionData
{
public:
    static std::unique_ptr<ProjectionData> Make( vtkSmartPointer<vtkImageData> p_volume, const std::vector<Position3D> & p_sourcesPositions );
    ~ProjectionData() = default;

    void GetDimensions( dim3 & p_dimension ) const;
    void GetOrigin( float3 & p_origin ) const;
    void GetSpacing( float2 & p_spacing ) const;

private:
    ProjectionData( vtkSmartPointer<vtkImageData> p_projections, const std::vector<Position3D> & p_sourcesPositions );

    vtkSmartPointer<vtkImageData> m_projections;
    std::vector<float3> m_sourcesPositions;
};
