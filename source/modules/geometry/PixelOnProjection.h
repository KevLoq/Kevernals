#pragma once

#include "modules/geometry/Pixel.h"


struct PixelOnProjection
{
    PixelOnProjection( int p_x, int p_y, int p_projectionIndex )
      : pixel( p_x, p_y )
      , projectionIndex( p_projectionIndex )
    {}

    PixelOnProjection( Pixel p_pixel, int p_projectionIndex )
      : pixel( p_pixel )
      , projectionIndex( p_projectionIndex )
    {}

    Pixel pixel;
    int projectionIndex{ 0 };
};


inline bool operator==( PixelOnProjection const & p_pixelOnProjectionA, PixelOnProjection const & p_pixelOnProjectionB )
{
    bool out = true;
    out &= ( p_pixelOnProjectionA.pixel == p_pixelOnProjectionB.pixel );
    out &= ( p_pixelOnProjectionA.projectionIndex == p_pixelOnProjectionB.projectionIndex );
    return out;
}

inline bool operator!=( PixelOnProjection const & p_pixelOnProjectionA, PixelOnProjection const & p_pixelOnProjectionB )
{
    return !( p_pixelOnProjectionA == p_pixelOnProjectionB );
}
