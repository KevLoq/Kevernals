#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"
#include "modules/reconstruction/ITomosynthesisReconstructor.h"

class TomosynthesisFBPReconstructor : public ITomosynthesisReconstructor
{
public:
    TomosynthesisFBPReconstructor() { m_reconstructionMethod = TomosynthesisReconstructorMethod::FBP; }
    ~TomosynthesisFBPReconstructor() override = default;
    Result<ImageDataPtr> PerformReconstruction() override;
};
