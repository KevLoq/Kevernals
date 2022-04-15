#pragma once

#include "modules/projection/IProjector.h"

#include "modules/projection/ShiftAndAddProjector.h"


std::unique_ptr<IProjector> IProjector::MakeProjector( TomoGeometry * p_tomoGeometry, ProjectorMethod p_method )
{
    if( p_tomoGeometry != nullptr )
    {
        if( p_method == ProjectorMethod::ShiftAndAdd )
        {
            return std::make_unique<ShiftAndAddProjector>(p_tomoGeometry); 
        } 
    }
    return std::make_unique<NullProjector>();
}
