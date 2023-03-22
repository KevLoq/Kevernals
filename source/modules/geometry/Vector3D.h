#pragma once

#include "modules/geometry/Dim3Vectorial.h"

struct Position3D;
struct Vector3D : public Dim3Vectorial<float>
{  
    Vector3D(float p_x,float p_y,float p_z): Dim3Vectorial<float>(p_x,p_y,p_z) {} ;
    ~Vector3D() override = default;
    Vector3D( const Position3D & p_origin, const Position3D & p_destination ) ;
    float NormL1() const;
    float NormL2() const;
};

 
