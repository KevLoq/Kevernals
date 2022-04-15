#pragma once

#include "modules/projection/IProjector.h"

#include "modules/projection/RayTracingProjector.h"
#include "modules/projection/ShiftAndAddProjector.h"


std::unique_ptr<IProjector> IProjector::MakeProjector( ProjectorMethod p_method )
{
    if( p_method == ProjectorMethod::ShiftAndAdd )
    {
        return std::make_unique<ShiftAndAddProjector>();
        // return std::make_unique<NullTomosynthesisReconstructor>();
    }
    else if( p_method == ProjectorMethod::RayTracing )
    {
        return std::make_unique<RayTracingProjector>();
    }
    return std::make_unique<NullProjector>();
}
