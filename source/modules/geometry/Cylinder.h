#pragma once

#include "modules/geometry/Position3D.h"


// enum for orientation easing
enum class Axis : int
{
    X = 0,
    Y,
    Z
};


struct Cylinder
{
    Position3D basisCenter;
    Axis orientation{ Axis::Z };
    float basisRadius{ 1.f };
    float height{ 1.f };

    Cylinder() = default;

    Cylinder( Position3D p_basisCenter, Axis p_orientation, float p_basisRadius, float p_height )
      : basisCenter{ p_basisCenter }
      , orientation( p_orientation )
      , basisRadius( p_basisRadius )
      , height( p_height )
    {}

    Cylinder( float p_x, float p_y, float p_z, Axis p_orientation, float p_basisRadius, float p_height )
      : basisCenter{ p_x, p_y, p_z }
      , orientation( p_orientation )
      , basisRadius( p_basisRadius )
      , height( p_height )
    {}


    Cylinder( const Cylinder & p_originalCylinder )
      : basisCenter( p_originalCylinder.basisCenter )
      , orientation( p_originalCylinder.orientation )
      , basisRadius( p_originalCylinder.basisRadius )
      , height( p_originalCylinder.height )
    {}

    Cylinder & operator=( const Cylinder & p_originalCylinder )
    {
        if( this != &p_originalCylinder )
        {
            basisCenter = p_originalCylinder.basisCenter;
            orientation = p_originalCylinder.orientation;
            basisRadius = p_originalCylinder.basisRadius;
            height = p_originalCylinder.height;
        }
        return *this;
    }

    bool Contains( const Position3D & p_point ) const
    {
        if( orientation == Axis::X )
        {
            auto heightMatches = p_point.x > basisCenter.x && p_point.x < basisCenter.x + height;
            if( !heightMatches )
            {
                return false;
            }
            auto lagY = basisCenter.y - p_point.y;
            auto lagZ = basisCenter.z - p_point.z;
            return ( lagY * lagY + lagZ * lagZ < basisRadius * basisRadius );
        }
        else if( orientation == Axis::Y )
        {
            auto heightMatches = p_point.y > basisCenter.y && p_point.y < basisCenter.y + height;
            if( !heightMatches )
            {
                return false;
            }
            auto lagX = basisCenter.x - p_point.x;
            auto lagZ = basisCenter.z - p_point.z;
            return ( lagX * lagX + lagZ * lagZ < basisRadius * basisRadius );
        }
        else if( orientation == Axis::Z )
        {
            auto heightMatches = p_point.z > basisCenter.z && p_point.z < basisCenter.z + height;
            if( !heightMatches )
            {
                return false;
            }
            auto lagY = basisCenter.y - p_point.y;
            auto lagX = basisCenter.x - p_point.x;
            return ( lagY * lagY + lagX * lagX < basisRadius * basisRadius );
        }
        return false;
    }
};

inline bool operator==( Cylinder const & p_cylinderA, Cylinder const & p_cylinderB )
{
    bool out = true;
    out &= p_cylinderA.basisCenter == p_cylinderB.basisCenter;
    out &= p_cylinderA.orientation == p_cylinderB.orientation;
    out &= AlmostEqualRelative( p_cylinderA.basisRadius, p_cylinderB.basisRadius );
    out &= AlmostEqualRelative( p_cylinderA.height, p_cylinderB.height );
    return out;
}

inline bool operator!=( Cylinder const & p_cylinderA, Cylinder const & p_cylinderB )
{
    return !( p_cylinderA == p_cylinderB );
}
