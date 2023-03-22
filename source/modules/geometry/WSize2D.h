#pragma once

#include "modules/geometry/Dim2.h"

struct PixelSpacing;
struct Size2D;
struct WSize2D : public Dim2<float>
{
    WSize2D(float p_x,float p_y): Dim2<float>(p_x,p_y) {} ;
    ~WSize2D() override = default;
    WSize2D( const PixelSpacing & p_pixelSpacing, const Size2D & p_size ) ;
};
