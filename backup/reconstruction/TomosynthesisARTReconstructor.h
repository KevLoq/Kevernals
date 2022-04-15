#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"
#include "modules/reconstruction/ITomosynthesisReconstructor.h"

class TomosynthesisARTReconstructor : public ITomosynthesisReconstructor
{
public:
    TomosynthesisARTReconstructor() { m_reconstructionMethod = TomosynthesisReconstructorMethod::ART; }
    ~TomosynthesisARTReconstructor() override = default;
    Result<ImageDataPtr> PerformReconstruction() override;
};
