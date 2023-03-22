#include "modules/geometry/Position2D.h"
#include "modules/geometry/Vector2D.h"
   
Vector2D::Vector2D( const Position2D & p_origin, const Position2D & p_destination )
 : Dim2Vectorial<float>(p_destination.x - p_origin.x,p_destination.y - p_origin.y) {}     
    
float Vector2D::NormL1() const
{
    return std::abs(x) + std::abs(y) ;    
}

float Vector2D::NormL2() const
{
    return std::sqrt( x*x + y*y);
}