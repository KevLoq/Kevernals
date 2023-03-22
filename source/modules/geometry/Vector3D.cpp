#include "modules/geometry/Position3D.h"
#include "modules/geometry/Vector3D.h"
   
Vector3D::Vector3D( const Position3D & p_origin, const Position3D & p_destination )
 : Dim3Vectorial<float>(p_destination.x - p_origin.x,p_destination.y - p_origin.y, p_destination.z - p_origin.z) {}  
  
float Vector3D::NormL1() const
{
    return std::abs(x) + std::abs(y) + std::abs(z);    
}

float Vector3D::NormL2() const
{
    return std::sqrt( x*x + y*y + z*z );
}