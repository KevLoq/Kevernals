#include "modules/geometry/Position3D.h"
#include "modules/geometry/Vector3D.h"
#include "modules/geometry/Voxel.h"

Position3D::Position3D(Voxel const & p_voxel):Position3D(static_cast<float>(p_voxel.x),static_cast<float>(p_voxel.y),static_cast<float>(p_voxel.z)){}

float Position3D::DistanceL2To( const Position3D & p_point ) const
{
     return (*this - p_point).NormL2();
}     

float Position3D::DistanceL1To( const Position3D & p_point ) const
{
     return (*this - p_point).NormL1();
}

Position3D & Position3D::operator+=( Vector3D const & p_rhs )
{
    x += p_rhs.x;
    y += p_rhs.y; 
    z += p_rhs.z; 
    return *this;
}

Vector3D operator-( Position3D const & p_lhs, Position3D const & p_rhs )
{
    return Vector3D (p_lhs.x - p_rhs.x,p_lhs.y - p_rhs.y,p_lhs.z - p_rhs.z);
}


Position3D operator+( Position3D p_lhs, Vector3D const & p_rhs )
{    
    p_lhs += p_rhs;
    return p_lhs;
}