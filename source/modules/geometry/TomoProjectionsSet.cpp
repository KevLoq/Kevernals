#pragma once

#include "modules/geometry/TomoProjectionsSet.h"

TomoProjectionsSet::TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const WSize2D & p_wsize, const Size2D & p_size )
  : m_bottomLeftPositions( p_bottomLeftpositions )
  , m_size( p_size )
  , m_wSize( p_wsize )
  , m_nProjections( m_bottomLeftPositions.size() )
{
    this->UpdatePixelSpacing();
    this->UpdatePositions();
}

TomoProjectionsSet::TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const WSize2D & p_wsize, const PixelSpacing & p_pixelSpacing )
  : m_bottomLeftPositions( p_bottomLeftpositions )
  , m_wSize( p_wsize )
  , m_pixelSpacing( p_pixelSpacing )
  , m_nProjections( m_bottomLeftPositions.size() )
{
    this->UpdateSize();
    this->UpdatePositions();
}

TomoProjectionsSet::TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const Size2D & p_size, const PixelSpacing & p_pixelSpacing )
  : m_bottomLeftPositions( p_bottomLeftpositions )
  , m_size( p_size )
  , m_pixelSpacing( p_pixelSpacing )
  , m_nProjections( m_bottomLeftPositions.size() )
{
    this->UpdateWSize();
    this->UpdatePositions();
}


void TomoProjectionsSet::SetPixelSpacingAndUpdateWSize( const PixelSpacing & p_pixelSpacing )
{
    if( m_pixelSpacing != p_pixelSpacing )
    {
        m_pixelSpacing = p_pixelSpacing;
        this->UpdateWSize();
        this->UpdatePositions();
    }
}

void TomoProjectionsSet::SetPixelSpacingAndUpdateSize( const PixelSpacing & p_pixelSpacing )
{
    if( m_pixelSpacing != p_pixelSpacing )
    {
        m_pixelSpacing = p_pixelSpacing;
        this->UpdateSize();
        this->UpdatePositions();
    }
}

void TomoProjectionsSet::SetSizeAndUpdatePixelSpacing( const Size2D & p_size )
{
    if( m_size != p_size )
    {
        m_size = p_size;
        this->UpdatePixelSpacing();
        this->UpdatePositions();
    }
}

void TomoProjectionsSet::SetSizeAndUpdateWSize( const Size2D & p_size )
{
    if( m_size != p_size )
    {
        m_size = p_size;
        this->UpdateWSize();
        this->UpdatePositions();
    }
}

void TomoProjectionsSet::SetWSizeAndUpdatePixelSpacing( const WSize2D & p_wsize )
{
    if( m_wSize != p_wsize )
    {
        m_wSize = p_wsize;
        this->UpdatePixelSpacing();
        this->UpdatePositions();
    }
}

void TomoProjectionsSet::SetWSizeAndUpdateSize( const WSize2D & p_wsize )
{
    if( m_wSize != p_wsize )
    {
        m_wSize = p_wsize;
        this->UpdateSize();
        this->UpdatePositions();
    }
}

TomoProjectionsSet & TomoProjectionsSet::operator=( const TomoProjectionsSet & p_originalTomoProjectionsSet )
{
    if( this != &p_originalTomoProjectionsSet )
    {
        m_bottomLeftPositions = p_originalTomoProjectionsSet.m_bottomLeftPositions;
        m_size = p_originalTomoProjectionsSet.m_size;
        m_wSize = p_originalTomoProjectionsSet.m_wSize;
        m_pixelSpacing = p_originalTomoProjectionsSet.m_pixelSpacing;
        m_xPositions = p_originalTomoProjectionsSet.m_xPositions;
        m_yPositions = p_originalTomoProjectionsSet.m_yPositions;
        m_projectionPositions = p_originalTomoProjectionsSet.m_projectionPositions;
        m_projectionPixelsIndices = p_originalTomoProjectionsSet.m_projectionPixelsIndices;
    }
    return *this;
}

std::vector<Position2D> TomoProjectionsSet::GetBottomLeftPositions() const
{
    return m_bottomLeftPositions;
}
Size2D TomoProjectionsSet::GetSize() const
{
    return m_size;
}
WSize2D TomoProjectionsSet::GetWSize() const
{
    return m_wSize;
}
PixelSpacing TomoProjectionsSet::GetPixelSpacing() const
{
    return m_pixelSpacing;
}
std::vector<std::vector<float>> TomoProjectionsSet::GetXPositions() const
{
    return m_xPositions;
}
std::vector<std::vector<float>> TomoProjectionsSet::GetYPositions() const
{
    return m_yPositions;
}
std::vector<Position2D> TomoProjectionsSet::GetProjectionPositions() const
{
    return m_projectionPositions;
}
std::vector<int> TomoProjectionsSet::GetProjectionPixelsIndices() const
{
    return m_projectionPixelsIndices;
}
// std::vector<PixelOnProjection> TomoProjectionsSet::GetProjectionIndiceAndPixels() const
//{
//     return m_projectionIndiceAndPixels;
// }
int TomoProjectionsSet::GetNProjections() const
{
    return m_nProjections;
}
Position2D TomoProjectionsSet::GetPosition( int p_projectionIndex, Pixel p_pixelPosition ) const
{
    return Position2D( m_xPositions[p_projectionIndex][p_pixelPosition.x], m_yPositions[p_projectionIndex][p_pixelPosition.y] );
}
Pixel TomoProjectionsSet::GetPixel( int p_projectionIndex, Position2D p_pixelPosition ) const
{
    Pixel pixel( 0, 0 );
    pixel.x = static_cast<int>( ( p_pixelPosition.x - m_bottomLeftPositions.at( p_projectionIndex ).x ) / m_pixelSpacing.x );
    pixel.y = static_cast<int>( ( p_pixelPosition.y - m_bottomLeftPositions.at( p_projectionIndex ).y ) / m_pixelSpacing.y );
    return pixel;
}

bool TomoProjectionsSet::ContainsInside( const Position2D & p_point, int p_projectionIndex ) const
{
    auto bottomLeftProjection = m_bottomLeftPositions.at( p_projectionIndex );

    return ( p_point.x > bottomLeftProjection.x || AlmostEqualRelative( p_point.x, bottomLeftProjection.x ) ) && p_point.x < ( bottomLeftProjection.x + m_wSize.x )
           && ( p_point.y > bottomLeftProjection.y || AlmostEqualRelative( p_point.y, bottomLeftProjection.y ) ) && p_point.y < ( bottomLeftProjection.y + m_wSize.y );
}

void TomoProjectionsSet::UpdatePositions()
{
    m_xPositions.clear();
    m_xPositions.resize( m_nProjections );
    m_yPositions.clear();
    m_yPositions.resize( m_nProjections );
    auto projectionPixelIndex{ 0 };
    for( auto projectionIndex{ 0 }; projectionIndex < m_nProjections; projectionIndex++ )
    {
        m_xPositions[projectionIndex].clear();
        m_xPositions[projectionIndex].resize( m_size.x );
        m_xPositions[projectionIndex][0] = m_bottomLeftPositions[projectionIndex].x;
        for( auto xIndex{ 1 }; xIndex < m_size.x; xIndex++ )
        {
            m_xPositions[projectionIndex][xIndex] = m_xPositions[projectionIndex][xIndex - 1] + m_pixelSpacing.x;
        }
        m_yPositions[projectionIndex].clear();
        m_yPositions[projectionIndex].resize( m_size.y );
        m_yPositions[projectionIndex][0] = m_bottomLeftPositions[projectionIndex].y;
        for( auto yIndex{ 1 }; yIndex < m_size.y; yIndex++ )
        {
            m_yPositions[projectionIndex][yIndex] = m_yPositions[projectionIndex][yIndex - 1] + m_pixelSpacing.y;
        }
    }

    m_projectionPositions.clear();
    m_projectionPixelsIndices.clear();
    // m_projectionIndiceAndPixels.clear();
    for( auto projectionIndex{ 0 }; projectionIndex < m_nProjections; projectionIndex++ )
    {
        for( auto yIndex{ 0 }; yIndex < m_size.y; yIndex++ )
        {
            for( auto xIndex{ 0 }; xIndex < m_size.x; xIndex++ )
            {
                m_projectionPixelsIndices.push_back( projectionPixelIndex );
                m_projectionPositions.push_back( Position2D( m_xPositions[projectionIndex][xIndex], m_yPositions[projectionIndex][yIndex] ) );
                // m_projectionIndiceAndPixels.push_back( PixelOnProjection( Pixel( xIndex, yIndex ), projectionIndex ) );
                projectionPixelIndex++;
            }
        }
    }
}

void TomoProjectionsSet::RemoveProjection( std::vector<int> const & p_dataIndicesToRemove )
{
    auto indices = p_dataIndicesToRemove;
    std::sort( std::begin( indices ), std::end( indices ), std::greater<int>() );
    std::cout << "RemoveProjection" << std::endl;
    for( auto index : indices )
    {
        std::cout << index << std::endl;
        if( index < 0 || index >= m_nProjections )
        {
            continue;
        }
        m_bottomLeftPositions.erase( std::begin( m_bottomLeftPositions ) + index );
    }
    m_nProjections = static_cast<int>( m_bottomLeftPositions.size() );

    this->UpdatePositions();
}

void TomoProjectionsSet::UpdateWSize()
{
    m_wSize = WSize2D( m_pixelSpacing, m_size );
}
void TomoProjectionsSet::UpdateSize()
{
    bool fake;
    m_size = Size2D::FromPixelSpacingAndSizeInWorld( m_pixelSpacing, m_wSize, fake );
}
void TomoProjectionsSet::UpdatePixelSpacing()
{
    bool fake;
    m_pixelSpacing = PixelSpacing::FromSizes( m_size, m_wSize, fake );
}

inline bool operator==( TomoProjectionsSet const & p_tomoProjectionsSetA, TomoProjectionsSet const & p_tomoProjectionsSetB )
{
    bool out = true;
    out &= p_tomoProjectionsSetA.GetBottomLeftPositions() == p_tomoProjectionsSetA.GetBottomLeftPositions();
    out &= p_tomoProjectionsSetA.GetWSize() == p_tomoProjectionsSetA.GetWSize();
    out &= p_tomoProjectionsSetA.GetSize() == p_tomoProjectionsSetA.GetSize();
    return out;
}

inline bool operator!=( TomoProjectionsSet const & p_tomoProjectionsSetA, TomoProjectionsSet const & p_tomoProjectionsSetB )
{
    return !( p_tomoProjectionsSetA == p_tomoProjectionsSetB );
}