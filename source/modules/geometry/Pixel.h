#pragma once

#include "modules/geometry/Dim2.h"

struct Pixel : public Dim2<int>
{ 
    Pixel(Dim2<int> const & p_pixel): Pixel(p_pixel.x,p_pixel.y) {}
    Pixel(int p_x,int p_y): Dim2<int>(p_x,p_y) {}
    ~Pixel() override = default;
    int DistanceL1To( Pixel const & p_point ) const;
    float DistanceL2To( Pixel const & p_point ) const; 
};
