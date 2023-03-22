#include "modules/geometry/Pixel.h"
#include "modules/geometry/Position2D.h"
 
float Pixel::DistanceL2To( Pixel const & p_point ) const
{
    return Position2D(*this).DistanceL2To(Position2D(p_point)) ;
}     

int Pixel::DistanceL1To( Pixel const & p_point ) const
{
       return Position2D(*this).DistanceL1To(Position2D(p_point)) ;
}
