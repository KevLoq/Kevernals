#pragma once

#include "modules/geometry/TomoVolume.h"
#include "modules/geometry/Voxel.h"

TomoVolume::TomoVolume( const Position3D & p_bottomLeftFront, const WSize3D & p_wsize, const Size3D & p_size )
  : m_pave( p_bottomLeftFront, p_wsize )
  , m_size( p_size )
{
    this->UpdateVoxelSpacing();
    this->UpdatePositions();
}

TomoVolume::TomoVolume( const Position3D & p_bottomLeftFront, const WSize3D & p_wsize, const VoxelSpacing & p_voxelSpacing )
  : m_pave( p_bottomLeftFront, p_wsize )
  , m_voxelSpacing( p_voxelSpacing )
{
    this->UpdateSize();
    this->UpdatePositions();
}

TomoVolume::TomoVolume( const Position3D & p_bottomLeftFront, const Size3D & p_size, const VoxelSpacing & p_voxelSpacing )
  : m_size( p_size )
  , m_voxelSpacing( p_voxelSpacing )
{
    m_pave.bottomLeftFront = p_bottomLeftFront;
    this->UpdateWSize();
    this->UpdatePositions();
}

TomoVolume::TomoVolume( const Pave & p_pave, const Size3D & p_size )
  : m_pave( p_pave )
  , m_size( p_size )
{
    this->UpdateVoxelSpacing();
    this->UpdatePositions();
}

TomoVolume::TomoVolume( const Pave & p_pave, const VoxelSpacing & p_voxelSpacing )
  : m_pave( p_pave )
  , m_voxelSpacing( p_voxelSpacing )
{
    this->UpdateSize();
    this->UpdatePositions();
}

void TomoVolume::SetVoxelSpacingAndUpdateWSize( const VoxelSpacing & p_voxelSpacing )
{
    if( m_voxelSpacing != p_voxelSpacing )
    {
        m_voxelSpacing = p_voxelSpacing;
        this->UpdateWSize();
        this->UpdatePositions();
    }
}

void TomoVolume::SetVoxelSpacingAndUpdateSize( const VoxelSpacing & p_voxelSpacing )
{
    if( m_voxelSpacing != p_voxelSpacing )
    {
        m_voxelSpacing = p_voxelSpacing;
        this->UpdateSize();
        this->UpdatePositions();
    }
}

void TomoVolume::SetSizeAndUpdateVoxelSpacing( const Size3D & p_size )
{
    if( m_size != p_size )
    {
        m_size = p_size;
        this->UpdateVoxelSpacing();
        this->UpdatePositions();
    }
}

void TomoVolume::SetSizeAndUpdateWSize( const Size3D & p_size )
{
    if( m_size != p_size )
    {
        m_size = p_size;
        this->UpdateWSize();
        this->UpdatePositions();
    }
}

void TomoVolume::SetWSizeAndUpdateVoxelSpacing( const WSize3D & p_wsize )
{
    if( m_pave.wsize != p_wsize )
    {
        m_pave.wsize = p_wsize;
        this->UpdateVoxelSpacing();
        this->UpdatePositions();
    }
}

void TomoVolume::SetWSizeAndUpdateSize( const WSize3D & p_wsize )
{
    if( m_pave.wsize != p_wsize )
    {
        m_pave.wsize = p_wsize;
        this->UpdateSize();
        this->UpdatePositions();
    }
}
TomoVolume & TomoVolume::operator=( const TomoVolume & p_originalTomoVolume )
{
    if( this != &p_originalTomoVolume )
    {
        m_pave = p_originalTomoVolume.m_pave;
        m_size = p_originalTomoVolume.m_size;
        m_voxelSpacing = p_originalTomoVolume.m_voxelSpacing;
        m_xPositions = p_originalTomoVolume.m_xPositions;
        m_yPositions = p_originalTomoVolume.m_yPositions;
        m_zPositions = p_originalTomoVolume.m_zPositions;
        m_gridPositions = p_originalTomoVolume.m_gridPositions;
        m_voxelsIndices = p_originalTomoVolume.m_voxelsIndices;
    }
    return *this;
}

bool TomoVolume::Contains( const Position3D & p_point ) const
{
    return m_pave.Contains( p_point );
}
bool TomoVolume::ContainsInsideVoxelGrid( const Position3D & p_point ) const
{
    return m_pave.ContainsInsideVoxelGrid( p_point );
}
int TomoVolume::GetVoxelsNumber() const
{
    return m_size.x * m_size.y * m_size.z;
}
float TomoVolume::GetSize() const
{
    return m_pave.GetVolumeSize();
}

Voxel TomoVolume::FindVoxelContainingPosition( const Position3D & p_position ) const
{
    Voxel voxel(0,0,0);
    voxel.x = static_cast<int>( ( p_position.x - m_pave.bottomLeftFront.x ) / m_voxelSpacing.x );
    voxel.y = static_cast<int>( ( p_position.y - m_pave.bottomLeftFront.y ) / m_voxelSpacing.y );
    voxel.z = static_cast<int>( ( p_position.z - m_pave.bottomLeftFront.z ) / m_voxelSpacing.z );
    return voxel;
}

Size3D TomoVolume::GetSize3D() const
{
    return m_size;
}
WSize3D TomoVolume::GetWSize3D() const
{
    return m_pave.wsize;
}
Position3D TomoVolume::GetBottomLeftFront() const
{
    return m_pave.bottomLeftFront;
}
Position3D TomoVolume::GetTopRightBack() const
{
    return m_pave.GetTopRightBack();
}
VoxelSpacing TomoVolume::GetVoxelSpacing() const
{
    return m_voxelSpacing;
}

// for computation optimizations
std::vector<float>  TomoVolume::GetXPositions() const
{
    return m_xPositions;
}
std::vector<float>  TomoVolume::GetYPositions() const
{
    return m_yPositions;
}
std::vector<float> TomoVolume::GetZPositions() const
{
    return m_zPositions;
}

std::vector<Position3D>  TomoVolume::GetGridPositions() const
{
    return m_gridPositions;
}
std::vector<int>  TomoVolume::GetVoxelsIndices() const
{
    return m_voxelsIndices;
}


void TomoVolume::UpdatePositions()
{
    m_xPositions.clear();
    m_xPositions.resize( m_size.x );
    m_xPositions[0] = m_pave.bottomLeftFront.x;
    for( auto i{ 1 }; i < m_size.x; i++ )
    {
        m_xPositions[i] = m_xPositions[i - 1] + m_voxelSpacing.x;
    }
    m_yPositions.clear();
    m_yPositions.resize( m_size.y );
    m_yPositions[0] = m_pave.bottomLeftFront.y;
    for( auto i{ 1 }; i < m_size.y; i++ )
    {
        m_yPositions[i] = m_yPositions[i - 1] + m_voxelSpacing.y;
    }
    m_zPositions.clear();
    m_zPositions.resize( m_size.z );
    m_zPositions[0] = m_pave.bottomLeftFront.z;
    for( auto i{ 1 }; i < m_size.z; i++ )
    {
        m_zPositions[i] = m_zPositions[i - 1] + m_voxelSpacing.z;
    }
    m_voxelsIndices.clear();
    m_gridPositions.clear();
    auto voxelIndex{ 0 };
    for( auto zIndex{ 0 }; zIndex < m_size.z; zIndex++ )
    {
        for( auto yIndex{ 0 }; yIndex < m_size.y; yIndex++ )
        {
            for( auto xIndex{ 0 }; xIndex < m_size.x; xIndex++ )
            {
                m_voxelsIndices.push_back( voxelIndex );
                m_gridPositions.push_back( Position3D( m_xPositions[xIndex], m_yPositions[yIndex], m_zPositions[zIndex] ) );
                voxelIndex++;
            }
        }
    }
}

void TomoVolume::UpdateWSize()
{
    m_pave.wsize = WSize3D( m_voxelSpacing,m_size );
}
void TomoVolume::UpdateSize()
{
    bool fake;
    m_size = Size3D::FromVoxelSpacingAndSizeInWorld( m_voxelSpacing, m_pave.wsize, fake );
}
void TomoVolume::UpdateVoxelSpacing()
{
    bool fake;
    m_voxelSpacing = VoxelSpacing::FromSizes( m_size, m_pave.wsize, fake );
}

inline bool operator==( TomoVolume const & p_volumeA, TomoVolume const & p_volumeB )
{
    bool out = true;
    out &= p_volumeA.GetWSize3D() == p_volumeB.GetWSize3D();
    out &= p_volumeA.GetBottomLeftFront() == p_volumeB.GetBottomLeftFront();
    out &= p_volumeA.GetSize() == p_volumeB.GetSize();
    return out;
}

inline bool operator!=( TomoVolume const & p_volumeA, TomoVolume const & p_volumeB )
{
    return !( p_volumeA == p_volumeB );
}