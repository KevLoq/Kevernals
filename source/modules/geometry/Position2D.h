#pragma once

#include "modules/geometry/Dim2.h" 

struct Vector2D;
struct Pixel;
struct Position2D : public Dim2<float>
{  
    Position2D(float p_x,float p_y): Dim2<float>(p_x,p_y) {}
    explicit Position2D(Pixel const & p_pixel);
    ~Position2D() override = default;
    float DistanceL1To( const Position2D & p_point ) const;
    float DistanceL2To( const Position2D & p_point ) const;
    Position2D & operator+=( Vector2D const & p_rhs );
    friend Vector2D operator-( Position2D const & p_lhs, Position2D const & p_rhs );
    friend Position2D operator+( Position2D p_lhs, Vector2D const & p_rhs ); // only right for vector Let A, B be Position3D, let u be a vector3D: B = A + u is ok but B = u + A is not
};

 
