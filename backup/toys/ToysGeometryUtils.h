#pragma once

#include "commons/Maths.h"

#include <QVector>

#include <cmath>

/*********************
   Basic 3D classes
**********************/


struct Position3D
{
    float x{ 0.f };
    float y{ 0.f };
    float z{ 0.f };

    Position3D() = default;

    Position3D( float p_x, float p_y, float p_z )
      : x( p_x )
      , y( p_y )
      , z( p_z )
    {}

    Position3D( Position3D && p_position )
      : x( std::move( p_position.x ) )
      , y( std::move( p_position.y ) )
      , z( std::move( p_position.z ) )
    {}

    Position3D( const Position3D & p_position )
      : x( p_position.x )
      , y( p_position.y )
      , z( p_position.z )
    {}

    Position3D & operator=( const Position3D & p_other )
    {
        if( this != &p_other )
        {
            x = p_other.x;
            y = p_other.y;
            z = p_other.z;
        }
        return *this;
    }

    Position3D & operator=( Position3D && p_other )
    {
        if( this != &p_other )
        {
            x = std::move( p_other.x );
            y = std::move( p_other.y );
            z = std::move( p_other.z );
        }
        return *this;
    }
    Position3D & operator+=( const Position3D & p_rhs )
    {
        x += p_rhs.x;
        y += p_rhs.y;
        z += p_rhs.z;
        return *this;
    }

    Position3D & operator-=( const Position3D & p_rhs )
    {
        x -= p_rhs.x;
        y -= p_rhs.y;
        z -= p_rhs.z;
        return *this;
    }

    friend Position3D operator+( Position3D p_lhs, const Position3D & p_rhs )
    {
        p_lhs += p_rhs;
        return p_lhs;
    }

    friend Position3D operator-( Position3D p_lhs, const Position3D & p_rhs )
    {
        p_lhs -= p_rhs;
        return p_lhs;
    }

    float DistanceTo( const Position3D & p_point ) const
    {
        auto diff = *this - p_point;
        return std::sqrt( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );
    }
};


inline bool operator==( Position3D const & p_Position3DA, Position3D const & p_Position3DB )
{
    bool out = true;
    out &= AlmostEqualRelative( p_Position3DA.x, p_Position3DB.x );
    out &= AlmostEqualRelative( p_Position3DA.y, p_Position3DB.y );
    out &= AlmostEqualRelative( p_Position3DA.z, p_Position3DB.z );
    return out;
}

inline bool operator!=( Position3D const & p_Position3DA, Position3D const & p_Position3DB )
{
    return !( p_Position3DA == p_Position3DB );
}
