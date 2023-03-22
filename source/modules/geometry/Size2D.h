#pragma once

#include "modules/geometry/Dim2.h"

struct PixelSpacing;
struct WSize2D;
struct Size2D : public Dim2<int>
{  
    Size2D(int p_x,int p_y): Dim2<int>(p_x,p_y) {}
    ~Size2D() override = default;
    static Size2D FromPixelSpacingAndSizeInWorld( const PixelSpacing & p_pixelSpacing, const WSize2D & p_sizeInWorld, bool & p_allDimensionsValid );
};
