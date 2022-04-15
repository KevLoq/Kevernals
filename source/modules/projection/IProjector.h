#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"
#include "modules/geometry/TomoGeometry.h"
#include "modules/projection/ProjectorErrorCode.h"

#include <memory>


enum class ProjectorMethod
{
    NO,
    ShiftAndAdd
};

class IProjector
{
public:
    virtual ~IProjector() = default;

    static std::unique_ptr<IProjector> MakeProjector( TomoGeometry * p_tomoGeometry, ProjectorMethod p_method );
 
    // PerformProjection PerformBackprojection methods are temporary
    // we should not perform projection here
    // A Projector class with different weighting
    // methods (linear, aerial, binary) should
    // be designed
    virtual Result<ImageDataPtr> PerformProjection( ImageDataPtr p_volume ) = 0;
    virtual Result<ImageDataPtr> PerformBackprojection( ImageDataPtr p_projection ) = 0;
    ProjectorMethod GetProjectorMethod() const { return m_projectorMethod; }


protected:

    IProjector(TomoGeometry * p_tomoGeometry) : m_tomoGeometry{ p_tomoGeometry}{};
    // Make it optional to check in PerformReconstruction method if we can do it...
    TomoGeometry * m_tomoGeometry;
    ProjectorMethod m_projectorMethod{ ProjectorMethod::ShiftAndAdd };
};


class NullProjector : public IProjector
{
public:
    NullProjector() : IProjector{nullptr} 
    {  
         m_projectorMethod = ProjectorMethod::NO; 
    }
    ~NullProjector() override = default;
    Result<ImageDataPtr> PerformProjection( ImageDataPtr p_volume ) override { return ProjectorErrorCode::BadAlgorithm; }
    Result<ImageDataPtr> PerformBackprojection( ImageDataPtr p_volume ) override { return ProjectorErrorCode::BadAlgorithm; } 
};