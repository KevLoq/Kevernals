#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"
#include "modules/projection/IProjector.h"

class ShiftAndAddProjector : public IProjector
{
public:
    ShiftAndAddProjector() { m_projectorMethod = ProjectorMethod::ShiftAndAdd; }
    ~ShiftAndAddProjector() override = default;

    Result<void> SetGeometry( const TomoGeometry & p_tomoGeometry ) override;

    Result<ImageDataPtr> PerformProjection( ImageDataPtr p_volume ) override;
    Result<ImageDataPtr> PerformBackprojection( ImageDataPtr p_projection ) override;


private:
    Result<void> ComputeTranslations();

    QVector<QVector<int>> m_projectionTranslationsForBackProjection;
    QVector<QVector<int>> m_reconstructionSlicesTranslationsForProjectionOperation;
};
