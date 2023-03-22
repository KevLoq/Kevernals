#pragma once

#include "modules/geometry/Voxel.h"
#include "modules/geometry/Size3D.h" 


struct Roi3D
{
    Voxel bottomLeftFront{0,0,0};
    Size3D size{0,0,0};

    Roi3D() = default;

    Roi3D( Voxel p_bottomLeftFront, Size3D p_size )
      : bottomLeftFront( p_bottomLeftFront ) 
      , size( p_size )
    {}

    Roi3D( int p_x, int p_y, int p_z, int p_xSize, int p_ySize, int p_zSize )
      : bottomLeftFront( p_x, p_y, p_z )
      , size( p_xSize, p_ySize, p_zSize )
    {}

    Roi3D( const Roi3D & p_originalRoi3D )
      : bottomLeftFront( p_originalRoi3D.bottomLeftFront )
      , size( p_originalRoi3D.size )
    {}

    Roi3D & operator=( const Roi3D & p_originalRoi3D )
    {
        if( this != &p_originalRoi3D )
        {
            bottomLeftFront = p_originalRoi3D.bottomLeftFront;
            size = p_originalRoi3D.size;
        }
        return *this;
    }

    Voxel GetTopRightBack() const
    {
        return Voxel{bottomLeftFront.x + size.x,bottomLeftFront.y + size.y,bottomLeftFront.z + size.z};
    }

    int GetRoiSize() const { return size.x * size.y * size.z; }

    bool Contains( const Voxel & p_point ) const
    {
        return ( p_point.x >= bottomLeftFront.x )
               && ( p_point.x < bottomLeftFront.x + size.x )
               && ( p_point.y >= bottomLeftFront.y )
               && ( p_point.y < bottomLeftFront.y + size.y )
               && ( p_point.z >= bottomLeftFront.z  )
               && ( p_point.z < bottomLeftFront.z + size.z );
    }
 
};

inline bool operator==( Roi3D const & p_paveA, Roi3D const & p_paveB )
{
    bool out = true;
    out &= p_paveA.bottomLeftFront == p_paveB.bottomLeftFront;
    out &= p_paveA.size == p_paveB.size;
    return out;
}

inline bool operator!=( Roi3D const & p_paveA, Roi3D const & p_paveB )
{
    return !( p_paveA == p_paveB );
}
