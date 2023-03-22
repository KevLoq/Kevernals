#pragma once

#include "modules/geometry/Position3D.h"
#include "modules/geometry/WSize3D.h"
#include "modules/geometry/Vector3D.h"

#include <ostream>


struct Pave
{
    Position3D bottomLeftFront{0.F,0.F,0.F};
    WSize3D wsize{0.F,0.F,0.F};

    Pave() = default;

    Pave( Position3D p_bottomLeftFront, WSize3D p_wsize )
      : bottomLeftFront( p_bottomLeftFront ) 
      , wsize( p_wsize )
    {}

    Pave( float p_x, float p_y, float p_z, float p_xSize, float p_ySize, float p_zSize )
      : bottomLeftFront( p_x, p_y, p_z )
      , wsize( p_xSize, p_ySize, p_zSize )
    {}

    Pave( const Pave & p_originalPave )
      : bottomLeftFront( p_originalPave.bottomLeftFront )
      , wsize( p_originalPave.wsize )
    {}

    friend std::ostream &operator<<(std::ostream &p_outputStream, Pave const & p_data) ;

    Pave & operator=( const Pave & p_originalPave )
    {
        if( this != &p_originalPave )
        {
            bottomLeftFront = p_originalPave.bottomLeftFront;
            wsize = p_originalPave.wsize;
        }
        return *this;
    }

    Position3D GetTopRightBack() const
    {
        return Position3D{bottomLeftFront.x + wsize.x,bottomLeftFront.y + wsize.y,bottomLeftFront.z + wsize.z};
    }

    float GetVolumeSize() const { return wsize.x * wsize.y * wsize.z; }

    bool Contains( const Position3D & p_point ) const
    {
        return ( p_point.x > bottomLeftFront.x || AlmostEqualRelative( p_point.x, bottomLeftFront.x ) )
               && ( p_point.x < bottomLeftFront.x + wsize.x || AlmostEqualRelative( p_point.x, bottomLeftFront.x + wsize.x ) )
               && ( p_point.y > bottomLeftFront.y || AlmostEqualRelative( p_point.y, bottomLeftFront.y ) )
               && ( p_point.y < bottomLeftFront.y + wsize.y || AlmostEqualRelative( p_point.y, bottomLeftFront.y + wsize.y ) )
               && ( p_point.z > bottomLeftFront.z || AlmostEqualRelative( p_point.z, bottomLeftFront.z ) )
               && ( p_point.z < bottomLeftFront.z + wsize.z || AlmostEqualRelative( p_point.z, bottomLeftFront.z + wsize.z ) );
    }

    std::vector<Position3D> FindIntersectionPointsWithLine( const Position3D & p_origin, const Vector3D & p_rayVector ) const
    {
        std::vector<Position3D> intersectionPoints;

        // As a preliminary, we look for the in and out intersection points between the line and the volume
        Position3D  volumeIntersectionBottom{0.F,0.F,0.F},
                    volumeIntersectionTop{0.F,0.F,0.F},
                    volumeIntersectionLeft{0.F,0.F,0.F},
                    volumeIntersectionRight{0.F,0.F,0.F},
                    volumeIntersectionFront{0.F,0.F,0.F},
                    volumeIntersectionBack{0.F,0.F,0.F};

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
        volumeIntersectionTop.z = bottomLeftFront.z + wsize.z;
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
        volumeIntersectionRight.y = bottomLeftFront.y + wsize.y;
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
        volumeIntersectionBack.x = bottomLeftFront.x + wsize.x;
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
               && p_point.x <  bottomLeftFront.x + wsize.x
               && ( p_point.y > bottomLeftFront.y || AlmostEqualRelative( p_point.y, bottomLeftFront.y ) )
               && p_point.y <  bottomLeftFront.y + wsize.y
               && ( p_point.z > bottomLeftFront.z || AlmostEqualRelative( p_point.z, bottomLeftFront.z ) )
               && p_point.z <  bottomLeftFront.z + wsize.z;
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

inline std::ostream &operator<<(std::ostream &p_outputStream, Pave const & p_data) { 
    return p_outputStream << "Bottom left front position: " << p_data.bottomLeftFront << " and size: " << p_data.wsize ;
}
