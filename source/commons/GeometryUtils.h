#pragma once

#include "commons/Maths.h"

#include <cmath>

// enum for orientation easing
enum class Axis : int
{
    X = 0,
    Y,
    Z
};

/*********************
   Basic 3D classes
**********************/


struct Position3D
{
    float x{ 0.f };
    float y{ 0.f };
    float z{ 0.f };

    Position3D() = default;

    Position3D( const float & p_x, const float & p_y, const float & p_z )
      : x( p_x )
      , y( p_y )
      , z( p_z )
    {}


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

struct Voxel
{
    int x{ 0 };
    int y{ 0 };
    int z{ 0 };

    Voxel() = default;

    Voxel( int p_x, int p_y, int p_z )
      : x( p_x )
      , y( p_y )
      , z( p_z )
    {}

    Voxel & operator+=( const Voxel & p_rhs )
    {
        x += p_rhs.x;
        y += p_rhs.y;
        z += p_rhs.z;
        return *this;
    }

    Voxel & operator-=( const Voxel & p_rhs )
    {
        x -= p_rhs.x;
        y -= p_rhs.y;
        z -= p_rhs.z;
        return *this;
    }
    friend Voxel operator+( Voxel p_lhs, const Voxel & p_rhs )
    {
        p_lhs += p_rhs;
        return p_lhs;
    }

    friend Voxel operator-( Voxel p_lhs, const Voxel & p_rhs )
    {
        p_lhs -= p_rhs;
        return p_lhs;
    }
    float DistanceL2To( const Voxel & p_point ) const
    {
        auto diff = *this - p_point;
        return std::sqrt( static_cast<float>( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z ) );
    }

    int DistanceL1To( const Voxel & p_point ) const
    {
        auto diff = *this - p_point;
        return std::abs( diff.x ) + std::abs( diff.y ) + std::abs( diff.z );
    }
};


inline bool operator==( Voxel const & p_voxelA, Voxel const & p_voxelB )
{
    bool out = true;
    out &= ( p_voxelA.x == p_voxelB.x );
    out &= ( p_voxelA.y == p_voxelB.y );
    out &= ( p_voxelA.z == p_voxelB.z );
    return out;
}

inline bool operator!=( Voxel const & p_voxelA, Voxel const & p_voxelB )
{
    return !( p_voxelA == p_voxelB );
}

// 'Derived' 3D classes

using VoxelSpacing = Position3D;
using Vector3D = Position3D;
using WSize3D = Position3D;

using Size3D = Voxel;

inline VoxelSpacing ComputeVoxelSpacing( const WSize3D & p_wsize, const Size3D & p_size )
{
    VoxelSpacing voxelSpacing( 1.f, 1.f, 1.f );
    if( p_size.x > 0 )
    {
        voxelSpacing.x = p_wsize.x / static_cast<float>( p_size.x );
    }
    if( p_size.y > 0 )
    {
        voxelSpacing.y = p_wsize.y / static_cast<float>( p_size.y );
    }
    if( p_size.z > 0 )
    {
        voxelSpacing.z = p_wsize.z / static_cast<float>( p_size.z );
    }
    return voxelSpacing;
}

inline Size3D ComputeSize3D( const WSize3D & p_wsize, const VoxelSpacing & p_voxelSpacing )
{
    Size3D size;
    if( p_voxelSpacing.x > 0.0000000000001 )
    {
        size.x = static_cast<int>( p_wsize.x / p_voxelSpacing.x );
    }
    if( p_voxelSpacing.y > 0.0000000000001 )
    {
        size.y = static_cast<int>( p_wsize.y / p_voxelSpacing.y );
    }
    if( p_voxelSpacing.z > 0.0000000000001 )
    {
        size.z = static_cast<int>( p_wsize.z / p_voxelSpacing.z );
    }
    return size;
}

inline WSize3D ComputeWSize3D( const Size3D & p_size, const VoxelSpacing & p_voxelSpacing )
{
    WSize3D wsize;
    wsize.x = static_cast<float>( p_size.x ) * p_voxelSpacing.x;
    wsize.y = static_cast<float>( p_size.y ) * p_voxelSpacing.y;
    wsize.z = static_cast<float>( p_size.z ) * p_voxelSpacing.z;
    return wsize;
}
/*********************
   Basic 2D classes
**********************/

struct Position2D
{
    float x{ 0.f };
    float y{ 0.f };

    Position2D() = default;

    Position2D( float p_x, float p_y )
      : x( p_x )
      , y( p_y )
    {}

    Position2D & operator+=( const Position2D & p_rhs )
    {
        x += p_rhs.x;
        y += p_rhs.y;
        return *this;
    }

    Position2D & operator-=( const Position2D & p_rhs )
    {
        x -= p_rhs.x;
        y -= p_rhs.y;
        return *this;
    }

    friend Position2D operator+( Position2D p_lhs, const Position2D & p_rhs )
    {
        p_lhs += p_rhs;
        return p_lhs;
    }

    friend Position2D operator-( Position2D p_lhs, const Position2D & p_rhs )
    {
        p_lhs -= p_rhs;
        return p_lhs;
    }

    float DistanceTo( const Position2D & p_point ) const
    {
        auto diff = *this - p_point;
        return std::sqrt( diff.x * diff.x + diff.y * diff.y );
    }
};


inline bool operator==( Position2D const & p_positionIn2DWorldA, Position2D const & p_positionIn2DWorldB )
{
    bool out = true;
    out &= AlmostEqualRelative( p_positionIn2DWorldA.x, p_positionIn2DWorldB.x );
    out &= AlmostEqualRelative( p_positionIn2DWorldA.y, p_positionIn2DWorldB.y );
    return out;
}

inline bool operator!=( Position2D const & p_positionIn2DWorldA, Position2D const & p_positionIn2DWorldB )
{
    return !( p_positionIn2DWorldA == p_positionIn2DWorldB );
}

struct Pixel
{
    int x{ 0 };
    int y{ 0 };

    Pixel() = default;

    Pixel( int p_x, int p_y )
      : x( p_x )
      , y( p_y )
    {}

    Pixel & operator+=( const Pixel & p_rhs )
    {
        x += p_rhs.x;
        y += p_rhs.y;
        return *this;
    }

    Pixel & operator-=( const Pixel & p_rhs )
    {
        x -= p_rhs.x;
        y -= p_rhs.y;
        return *this;
    }

    friend Pixel operator+( Pixel p_lhs, const Pixel & p_rhs )
    {
        p_lhs += p_rhs;
        return p_lhs;
    }

    friend Pixel operator-( Pixel p_lhs, const Pixel & p_rhs )
    {
        p_lhs -= p_rhs;
        return p_lhs;
    }

    float DistanceL2To( const Pixel & p_point ) const
    {
        auto diff = *this - p_point;
        return std::sqrt( static_cast<float>( diff.x * diff.x + diff.y * diff.y ) );
    }

    int DistanceL1To( const Pixel & p_point ) const
    {
        auto diff = *this - p_point;
        return std::abs( diff.x ) + std::abs( diff.y );
    }
};


inline bool operator==( Pixel const & p_pixelA, Pixel const & p_pixelB )
{
    bool out = true;
    out &= ( p_pixelA.x == p_pixelB.x );
    out &= ( p_pixelA.y == p_pixelB.y );
    return out;
}

inline bool operator!=( Pixel const & p_pixelA, Pixel const & p_pixelB )
{
    return !( p_pixelA == p_pixelB );
}


// 'Derived' 2D classes

using PixelSpacing = Position2D;
using Vector2D = Position2D;
using WSize2D = Position2D;
using Size2D = Pixel;

inline PixelSpacing ComputePixelSpacing( const WSize2D & p_wsize, const Size2D & p_size )
{
    PixelSpacing pixelSpacing( 1.f, 1.f );
    if( p_size.x > 0 )
    {
        pixelSpacing.x = p_wsize.x / static_cast<float>( p_size.x );
    }
    if( p_size.y > 0 )
    {
        pixelSpacing.y = p_wsize.y / static_cast<float>( p_size.y );
    }
    return pixelSpacing;
}

inline Size2D ComputeSize2D( const WSize2D & p_wsize, const PixelSpacing & p_pixelSpacing )
{
    Size2D size;
    if( p_pixelSpacing.x > 0.0000000000001 )
    {
        size.x = static_cast<int>( p_wsize.x / p_pixelSpacing.x );
    }
    if( p_pixelSpacing.y > 0.0000000000001 )
    {
        size.y = static_cast<int>( p_wsize.y / p_pixelSpacing.y );
    }
    return size;
}

inline WSize2D ComputeWSize2D( const Size2D & p_size, const PixelSpacing & p_pixelSpacing )
{
    WSize2D wsize;
    wsize.x = static_cast<float>( p_size.x ) * p_pixelSpacing.x;
    wsize.y = static_cast<float>( p_size.y ) * p_pixelSpacing.y;
    return wsize;
}


struct PixelOnProjection
{
    PixelOnProjection( int p_x, int p_y, int p_projectionIndex )
      : pixel( p_x, p_y )
      , projectionIndex( p_projectionIndex )
    {}

    PixelOnProjection( Pixel p_pixel, int p_projectionIndex )
      : pixel( p_pixel )
      , projectionIndex( p_projectionIndex )
    {}

    Pixel pixel;
    int projectionIndex{ 0 };
};


inline bool operator==( PixelOnProjection const & p_pixelOnProjectionA, PixelOnProjection const & p_pixelOnProjectionB )
{
    bool out = true;
    out &= ( p_pixelOnProjectionA.pixel == p_pixelOnProjectionB.pixel );
    out &= ( p_pixelOnProjectionA.projectionIndex == p_pixelOnProjectionB.projectionIndex );
    return out;
}

inline bool operator!=( PixelOnProjection const & p_pixelOnProjectionA, PixelOnProjection const & p_pixelOnProjectionB )
{
    return !( p_pixelOnProjectionA == p_pixelOnProjectionB );
}

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

    Sphere( const Sphere & p_originalSphere )
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
        return ( center.DistanceTo( p_point ) < radius );
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

struct Pave
{
    Position3D bottomLeftFront;
    Position3D topRightBack;
    WSize3D wsize;

    Pave() = default;

    Pave( Position3D p_bottomLeftFront, WSize3D p_wsize )
      : bottomLeftFront( p_bottomLeftFront )
      , topRightBack( p_bottomLeftFront + p_wsize )
      , wsize( p_wsize )
    {}

    Pave( float p_x, float p_y, float p_z, float p_xSize, float p_ySize, float p_zSize )
      : bottomLeftFront( p_x, p_y, p_z )
      , wsize( p_xSize, p_ySize, p_zSize )
    {
        topRightBack = bottomLeftFront + wsize;
    }

    Pave( const Pave & p_originalPave )
      : bottomLeftFront( p_originalPave.bottomLeftFront )
      , topRightBack( p_originalPave.topRightBack )
      , wsize( p_originalPave.wsize )
    {}

    Pave & operator=( const Pave & p_originalPave )
    {
        if( this != &p_originalPave )
        {
            bottomLeftFront = p_originalPave.bottomLeftFront;
            topRightBack = p_originalPave.topRightBack;
            wsize = p_originalPave.wsize;
        }
        return *this;
    }

    float GetVolumeSize() const { return wsize.x * wsize.y * wsize.z; }

    bool Contains( const Position3D & p_point ) const
    {
        return ( p_point.x > bottomLeftFront.x || AlmostEqualRelative( p_point.x, bottomLeftFront.x ) )
               && ( p_point.x < topRightBack.x || AlmostEqualRelative( p_point.x, topRightBack.x ) )
               && ( p_point.y > bottomLeftFront.y || AlmostEqualRelative( p_point.y, bottomLeftFront.y ) )
               && ( p_point.y < topRightBack.y || AlmostEqualRelative( p_point.y, topRightBack.y ) )
               && ( p_point.z > bottomLeftFront.z || AlmostEqualRelative( p_point.z, bottomLeftFront.z ) )
               && ( p_point.z < topRightBack.z || AlmostEqualRelative( p_point.z, topRightBack.z ) );
    }

    std::vector<Position3D> FindIntersectionPointsWithLine( const Position3D & p_origin, const Vector3D & p_rayVector ) const
    {
        std::vector<Position3D> intersectionPoints;

        // As a preliminary, we look for the in and out intersection points between the line and the volume
        Position3D volumeIntersectionBottom, volumeIntersectionTop,
          volumeIntersectionLeft, volumeIntersectionRight,
          volumeIntersectionFront, volumeIntersectionBack;

        // Bottom check
        volumeIntersectionBottom.z = bottomLeftFront.z;
        auto parametricT = ( volumeIntersectionBottom.z - p_origin.z ) / p_rayVector.z;
        volumeIntersectionBottom.x = p_origin.x + parametricT * p_rayVector.x;
        volumeIntersectionBottom.y = p_origin.y + parametricT * p_rayVector.y;
        if( this->Contains( volumeIntersectionBottom ) )
        {
            intersectionPoints.push_back( volumeIntersectionBottom );
        }


        // Top check
        volumeIntersectionTop.z = topRightBack.z;
        parametricT = ( volumeIntersectionTop.z - p_origin.z ) / p_rayVector.z;
        volumeIntersectionTop.x = p_origin.x + parametricT * p_rayVector.x;
        volumeIntersectionTop.y = p_origin.y + parametricT * p_rayVector.y;
        if( this->Contains( volumeIntersectionTop ) )
        {
            intersectionPoints.push_back( volumeIntersectionTop );
        }

        // Left check
        volumeIntersectionLeft.y = bottomLeftFront.y;
        parametricT = ( volumeIntersectionLeft.y - p_origin.y ) / p_rayVector.y;
        volumeIntersectionLeft.x = p_origin.x + parametricT * p_rayVector.x;
        volumeIntersectionLeft.z = p_origin.z + parametricT * p_rayVector.z;
        if( this->Contains( volumeIntersectionLeft ) )
        {
            intersectionPoints.push_back( volumeIntersectionLeft );
        }

        // Right check
        volumeIntersectionRight.y = topRightBack.y;
        parametricT = ( volumeIntersectionRight.y - p_origin.y ) / p_rayVector.y;
        volumeIntersectionRight.x = p_origin.x + parametricT * p_rayVector.x;
        volumeIntersectionRight.z = p_origin.z + parametricT * p_rayVector.z;
        if( this->Contains( volumeIntersectionRight ) )
        {
            intersectionPoints.push_back( volumeIntersectionRight );
        }

        // Front check
        volumeIntersectionFront.x = bottomLeftFront.x;
        parametricT = ( volumeIntersectionFront.x - p_origin.x ) / p_rayVector.x;
        volumeIntersectionFront.y = p_origin.y + parametricT * p_rayVector.y;
        volumeIntersectionFront.z = p_origin.z + parametricT * p_rayVector.z;
        if( this->Contains( volumeIntersectionFront ) )
        {
            intersectionPoints.push_back( volumeIntersectionFront );
        }

        // Back check
        volumeIntersectionBack.x = topRightBack.x;
        parametricT = ( volumeIntersectionBack.x - p_origin.x ) / p_rayVector.x;
        volumeIntersectionBack.y = p_origin.y + parametricT * p_rayVector.y;
        volumeIntersectionBack.z = p_origin.z + parametricT * p_rayVector.z;
        if( this->Contains( volumeIntersectionBack ) )
        {
            intersectionPoints.push_back( volumeIntersectionBack );
        }
        return intersectionPoints;
    }


    bool ContainsInsideVoxelGrid( const Position3D & p_point ) const
    {
        return ( p_point.x > bottomLeftFront.x || AlmostEqualRelative( p_point.x, bottomLeftFront.x ) )
               && p_point.x < topRightBack.x
               && ( p_point.y > bottomLeftFront.y || AlmostEqualRelative( p_point.y, bottomLeftFront.y ) )
               && p_point.y < topRightBack.y
               && ( p_point.z > bottomLeftFront.z || AlmostEqualRelative( p_point.z, bottomLeftFront.z ) )
               && p_point.z < topRightBack.z;
    }
};

inline bool operator==( Pave const & p_paveA, Pave const & p_paveB )
{
    bool out = true;
    out &= p_paveA.bottomLeftFront == p_paveB.bottomLeftFront;
    out &= p_paveA.wsize == p_paveB.wsize;
    return out;
}

inline bool operator!=( Pave const & p_paveA, Pave const & p_paveB )
{
    return !( p_paveA == p_paveB );
}
