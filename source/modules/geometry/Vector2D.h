#pragma once

#include "modules/geometry/Dim2Vectorial.h"

struct Position2D;
struct Vector2D : public Dim2Vectorial<float>
{  
    Vector2D(float p_x,float p_y): Dim2Vectorial<float>(p_x,p_y) {} ;
    ~Vector2D() override = default;
    Vector2D( const Position2D & p_origin, const Position2D & p_destination ) ;
    float NormL1() const;
    float NormL2() const;
};

 
