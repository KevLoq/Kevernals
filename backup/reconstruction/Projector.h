#pragma once

#include "commons/ImageDataPtr.h"
#include "modules/geometry/TomoGeometry.h"

#include <vtkTIFFWriter.h>

class Projector
{
public:
    Projector( TomoGeometry * p_tomoGeometry )
      : m_tomoGeometry{ p_tomoGeometry } {};
    ~Projector() = default;

    ImageDataPtr PerformProjection( ImageDataPtr p_volume ) const;
    ImageDataPtr PerformBackProjection( ImageDataPtr p_projections ) const;

private:
    // Make it optional to check in PerformReconstruction method if we can do it...
    TomoGeometry * m_tomoGeometry;
};
