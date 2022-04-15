#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"
#include "modules/geometry/RadonMatrixManager.h"
#include "modules/geometry/TomoGeometry.h"
#include "modules/projection/IProjector.h"
#include "modules/reconstruction/TomosynthesisReconstructorErrorCode.h"

#include <memory>


enum class TomosynthesisReconstructorMethod
{
    NO,
    FBP,
    ART
};

class ITomosynthesisReconstructor
{
public:
    ITomosynthesisReconstructor();
    virtual ~ITomosynthesisReconstructor() = default;

    static std::unique_ptr<ITomosynthesisReconstructor> MakeReconstructor( TomosynthesisReconstructorMethod p_method );

    Result<void> LoadProjectionData( const QString & p_projectionDataFilePath, bool p_performLogarithmicTransform = false );
    ImageDataPtr GetProjectionData() const;
    void SetProjectionData( ImageDataPtr p_projectionData );

    Result<void> SetProjectorMethod( ProjectorMethod p_method );
    TomosynthesisReconstructorMethod GetTomosynthesisReconstructorMethod() const { return m_reconstructionMethod; }

    Result<void> SetGeometry( const TomoGeometry & p_tomoGeometry );
    virtual Result<ImageDataPtr> PerformReconstruction() = 0;

protected:
    // Make it optional to check in PerformReconstruction method if we can do it...
    TomoGeometry m_tomoGeometry;
    std::unique_ptr<IProjector> m_projector{ nullptr };
    ImageDataPtr m_projectionData;

    TomosynthesisReconstructorMethod m_reconstructionMethod;
};


class NullTomosynthesisReconstructor : public ITomosynthesisReconstructor
{
public:
    NullTomosynthesisReconstructor() { m_reconstructionMethod = TomosynthesisReconstructorMethod::NO; }
    ~NullTomosynthesisReconstructor() override = default;
    Result<ImageDataPtr> PerformReconstruction() override { return TomosynthesisReconstructorErrorCode::BadAlgorithm; }
};