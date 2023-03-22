#include "modules/geometry/Position2D.h"
#include "modules/geometry/Vector2D.h"
#include "modules/geometry/Pixel.h"
   
Position2D::Position2D(Pixel const & p_pixel):Position2D(static_cast<float>(p_pixel.x),static_cast<float>(p_pixel.y)){}


float Position2D::DistanceL2To( const Position2D & p_point ) const
{
     return (*this - p_point).NormL2();
}     

float Position2D::DistanceL1To( const Position2D & p_point ) const
{
     return (*this - p_point).NormL1();
}

Position2D & Position2D::operator+=( Vector2D const & p_rhs )
{
    x += p_rhs.x;
    y += p_rhs.y; 
    return *this;
}

Vector2D operator-( Position2D const & p_lhs, Position2D const & p_rhs )
{
    return Vector2D (p_lhs.x - p_rhs.x,p_lhs.y - p_rhs.y);
}


Position2D operator+( Position2D p_lhs, Vector2D const & p_rhs )
{    
    p_lhs += p_rhs;
    return p_lhs;
}