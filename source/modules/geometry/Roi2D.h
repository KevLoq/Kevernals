#pragma once

#include "modules/geometry/Pixel.h"
#include "modules/geometry/Size2D.h" 


struct Roi2D
{
    Pixel bottomLeftFront{0,0};
    Size2D size{0,0};

    Roi2D() = default;

    Roi2D( Pixel p_bottomLeftFront, Size2D p_size )
      : bottomLeftFront( p_bottomLeftFront ) 
      , size( p_size )
    {}

    Roi2D( int p_x, int p_y, int p_xSize, int p_ySize )
      : bottomLeftFront( p_x, p_y )
      , size( p_xSize, p_ySize )
    {}

    Roi2D( const Roi2D & p_originalRoi2D )
      : bottomLeftFront( p_originalRoi2D.bottomLeftFront )
      , size( p_originalRoi2D.size )
    {}

    Roi2D & operator=( const Roi2D & p_originalRoi2D )
    {
        if( this != &p_originalRoi2D )
        {
            bottomLeftFront = p_originalRoi2D.bottomLeftFront;
            size = p_originalRoi2D.size;
        }
        return *this;
    }

    Pixel GetTopRightBack() const
    {
        return Pixel {bottomLeftFront.x + size.x,bottomLeftFront.y + size.y };
    }

    int GetRoiSize() const { return size.x * size.y ; }

    bool Contains( const Pixel & p_point ) const
    {
        return ( p_point.x >= bottomLeftFront.x )
               && ( p_point.x < bottomLeftFront.x + size.x )
               && ( p_point.y >= bottomLeftFront.y )
               && ( p_point.y < bottomLeftFront.y + size.y );
    }
 
};

inline bool operator==( Roi2D const & p_paveA, Roi2D const & p_paveB )
{
    bool out = true;
    out &= p_paveA.bottomLeftFront == p_paveB.bottomLeftFront;
    out &= p_paveA.size == p_paveB.size;
    return out;
}

inline bool operator!=( Roi2D const & p_paveA, Roi2D const & p_paveB )
{
    return !( p_paveA == p_paveB );
}
