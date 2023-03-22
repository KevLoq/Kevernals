#pragma once

#include "modules/geometry/Dim3.h" 

struct Vector3D;
struct Voxel;
struct Position3D : public Dim3<float>
{  
    Position3D(float p_x,float p_y,float p_z): Dim3<float>(p_x,p_y,p_z) {}
    explicit Position3D(Voxel const & p_voxel);
    ~Position3D() override = default;
    float DistanceL1To( const Position3D & p_point ) const;
    float DistanceL2To( const Position3D & p_point ) const;
    Position3D & operator+=( Vector3D const & p_rhs );
    friend Vector3D operator-( Position3D const & p_lhs, Position3D const & p_rhs );
    friend Position3D operator+( Position3D p_lhs, Vector3D const & p_rhs ); // only right for vector Let A, B be Position3D, let u be a vector3D: B = A + u is ok but B = u + A is not
};

 
