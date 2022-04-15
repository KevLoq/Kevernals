#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/RadonMatrices.h"
#include "commons/Result.h"
#include "modules/projection/IProjector.h"

class RayTracingProjector : public IProjector
{
public:
    RayTracingProjector() { m_projectorMethod = ProjectorMethod::RayTracing; }
    ~RayTracingProjector() override = default;

    Result<void> SetGeometry( const TomoGeometry & p_tomoGeometry ) override;

    Result<ImageDataPtr> PerformProjection( ImageDataPtr p_volume ) override;
    Result<ImageDataPtr> PerformBackprojection( ImageDataPtr p_projection ) override;

private:
    Result<void> ComputeRadonMatrices();
    Result<void> ComputeRadonProjectionMatrix();
    Result<void> ComputeRadonBackProjectionMatrix();
    // RadonProjectionMatrix and RadonBackProjectionMatrix
    // are not common to all reconstruction methods.
    // It should not be here

    RadonProjectionMatrix m_radonProjectionMatrix;
    RadonBackProjectionMatrix m_radonBackProjectionMatrix;
};
