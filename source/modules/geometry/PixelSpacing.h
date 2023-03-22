#pragma once

#include "modules/geometry/Dim2.h"

struct Size2D;
struct WSize2D;
struct PixelSpacing : public Dim2<float>
{
    PixelSpacing(float p_x,float p_y): Dim2<float>(p_x,p_y) {}
    ~PixelSpacing() override = default;
    static PixelSpacing FromSizes( const Size2D & p_dimensions, const WSize2D & p_sizeInWorld, bool & p_allDimensionsValid );
};
