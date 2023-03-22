#pragma once

#include "modules/geometry/Position3D.h"

struct Sphere
{
    Position3D center;
    float radius{ 1.f };

    Sphere() = default;

    Sphere( Position3D p_center, float p_radius )
      : center( p_center )
      , radius( p_radius )
    {}

    Sphere( float p_x, float p_y, float p_z, float p_radius )
      : center( p_x, p_y, p_z )
      , radius( p_radius )
    {}

    Sphere( Sphere const & p_originalSphere )
      : center{ p_originalSphere.center }
      , radius( p_originalSphere.radius )
    {}

    Sphere & operator=( const Sphere & p_originalSphere )
    {
        if( this != &p_originalSphere )
        {
            center = p_originalSphere.center;
            radius = p_originalSphere.radius;
        }
        return *this;
    }

    bool Contains( const Position3D & p_point ) const
    {
        return ( center.DistanceL2To( p_point ) < radius );
    }
};

inline bool operator==( Sphere const & p_sphereA, Sphere const & p_sphereB )
{
    bool out = true;
    out &= p_sphereA.center == p_sphereB.center;
    out &= AlmostEqualRelative( p_sphereA.radius, p_sphereB.radius );
    return out;
}

inline bool operator!=( Sphere const & p_sphereA, Sphere const & p_sphereB )
{
    return !( p_sphereA == p_sphereB );
}