#pragma once

#include "modules/geometry/TomoGeometry.h"

#include <vtkImageData.h>
#include <vtkSmartPointer.h>

struct dim3;
struct float3;
class Projector
{
public:
    Projector( TomoGeometry * p_tomoGeometry )
      : m_tomoGeometry{ p_tomoGeometry } {};
    ~Projector() = default;

    vtkSmartPointer<vtkImageData> PerformProjection( vtkSmartPointer<vtkImageData> p_volume ) const;
    void PerformProjection( const vtkSmartPointer<vtkImageData> p_volume, const Position3D & p_sourcePosition, vtkSmartPointer<vtkImageData> p_projectionsContainer ) const;
    vtkSmartPointer<vtkImageData> PerformBackProjection( vtkSmartPointer<vtkImageData> p_projections ) const;

private:
    void ExtractDimension( const vtkSmartPointer<vtkImageData> p_imageDataPtr, dim3 & p_dimension ) const;
    void ExtractSpacing( const vtkSmartPointer<vtkImageData> p_imageDataPtr, float3 & p_spacing ) const;
    void ExtractOrigin( const vtkSmartPointer<vtkImageData> p_imageDataPtr, float3 & p_origin ) const;

    // Make it optional to check in PerformReconstruction method if we can do it...
    TomoGeometry * m_tomoGeometry;
};
