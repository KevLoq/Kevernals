#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"
#include "modules/geometry/TomoGeometry.h"
#include "modules/projection/ProjectorErrorCode.h"

#include <memory>


enum class ProjectorMethod
{
    NO,
    ShiftAndAdd,
    RayTracing
};

class IProjector
{
public:
    IProjector() = default;
    virtual ~IProjector() = default;

    static std::unique_ptr<IProjector> MakeProjector( ProjectorMethod p_method );

    virtual Result<void> SetGeometry( const TomoGeometry & p_tomoGeometry ) = 0;

    // PerformProjection PerformBackprojection methods are temporary
    // we should not perform projection here
    // A Projector class with different weighting
    // methods (linear, aerial, binary) should
    // be designed
    virtual Result<ImageDataPtr> PerformProjection( ImageDataPtr p_volume ) = 0;
    virtual Result<ImageDataPtr> PerformBackprojection( ImageDataPtr p_projection ) = 0;
    ProjectorMethod GetProjectorMethod() const { return m_projectorMethod; }


protected:
    // Make it optional to check in PerformReconstruction method if we can do it...
    TomoGeometry m_tomoGeometry;
    ProjectorMethod m_projectorMethod{ ProjectorMethod::ShiftAndAdd };
};


class NullProjector : public IProjector
{
public:
    NullProjector() { m_projectorMethod = ProjectorMethod::NO; }
    ~NullProjector() override = default;
    Result<ImageDataPtr> PerformProjection( ImageDataPtr p_volume ) override { return ProjectorErrorCode::BadAlgorithm; }
    Result<ImageDataPtr> PerformBackprojection( ImageDataPtr p_volume ) override { return ProjectorErrorCode::BadAlgorithm; }
    Result<void> SetGeometry( const TomoGeometry & p_tomoGeometry ) override { return ProjectorErrorCode::BadAlgorithm; }
};