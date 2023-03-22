#include "modules/geometry/Size2D.h"

#include "modules/geometry/PixelSpacing.h"
#include "modules/geometry/WSize2D.h"

Size2D Size2D::FromPixelSpacingAndSizeInWorld( const PixelSpacing & p_pixelSpacing, const WSize2D & p_sizeInWorld, bool & p_allDimensionsValid )
{
    p_allDimensionsValid = true;
    Size2D size(0,0);
    if( p_pixelSpacing.x > 0.F )
    {
        size.x = static_cast<int>( p_sizeInWorld.x / p_pixelSpacing.x );
    }
    else
    {
        p_allDimensionsValid = false;
    }
    if( p_pixelSpacing.y > 0.F )
    {
        size.y = static_cast<int>( p_sizeInWorld.y / p_pixelSpacing.y );
    }
    else
    {
        p_allDimensionsValid = false;
    }
    return size;
}