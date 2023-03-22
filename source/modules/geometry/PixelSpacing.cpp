#include "modules/geometry/PixelSpacing.h"

#include "modules/geometry/Size2D.h"
#include "modules/geometry/WSize2D.h"

PixelSpacing PixelSpacing::FromSizes( const Size2D & p_dimensions, const WSize2D & p_sizeInWorld, bool & p_allDimensionsValid )
{
    p_allDimensionsValid = true;
    
    PixelSpacing pixelSpacing{1.F, 1.F};
    if( p_dimensions.x > 0 )
    {
        pixelSpacing.x = p_sizeInWorld.x / static_cast<float>( p_dimensions.x );
    }
    else {
        p_allDimensionsValid = false;
    }
    if( p_dimensions.y > 0 )
    {
        pixelSpacing.y = p_sizeInWorld.y / static_cast<float>( p_dimensions.y );
    }
    else {
        p_allDimensionsValid = false;
    }
    return pixelSpacing;
}
