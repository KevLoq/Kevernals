#pragma once

#include "modules/geometry/TomoVolume.h"

#include <QDebug>


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

std::vector<Position3D> TomoVolume::FindIntersectionPointsWithLine( const Position3D & p_origin, const Vector3D & p_rayVector ) const
{
    std::vector<Position3D> intersectionPoints;
    std::vector<Side> intersectionPointSides;
    std::vector<Voxel> intersectionPointVoxels;
    // As a preliminary, we look for the in and out intersection points between the line and the volume
    Position3D volumeIntersectionBottom, volumeIntersectionTop,
      volumeIntersectionLeft, volumeIntersectionRight,
      volumeIntersectionFront, volumeIntersectionBack;

    // Bottom check
    volumeIntersectionBottom.z = m_pave.bottomLeftFront.z;
    auto parametricT = ( volumeIntersectionBottom.z - p_origin.z ) / p_rayVector.z;
    volumeIntersectionBottom.x = p_origin.x + parametricT * p_rayVector.x;
    volumeIntersectionBottom.y = p_origin.y + parametricT * p_rayVector.y;
    if( this->Contains( volumeIntersectionBottom ) )
    {
        intersectionPoints.push_back( volumeIntersectionBottom );
        intersectionPointVoxels.push_back( this->FindVoxelContainingPosition( volumeIntersectionBottom ) );
        intersectionPointSides.push_back( Side::Bottom );
    }


    // Top check
    volumeIntersectionTop.z = m_pave.topRightBack.z;
    parametricT = ( volumeIntersectionTop.z - p_origin.z ) / p_rayVector.z;
    volumeIntersectionTop.x = p_origin.x + parametricT * p_rayVector.x;
    volumeIntersectionTop.y = p_origin.y + parametricT * p_rayVector.y;
    if( this->Contains( volumeIntersectionTop ) )
    {
        volumeIntersectionTop.z -= m_voxelSpacing.z;
        intersectionPointVoxels.push_back( this->FindVoxelContainingPosition( volumeIntersectionTop ) );
        volumeIntersectionTop.z += m_voxelSpacing.z;
        intersectionPoints.push_back( volumeIntersectionTop );
        intersectionPointSides.push_back( Side::Top );
    }

    // Left check
    volumeIntersectionLeft.y = m_pave.bottomLeftFront.y;
    parametricT = ( volumeIntersectionLeft.y - p_origin.y ) / p_rayVector.y;
    volumeIntersectionLeft.x = p_origin.x + parametricT * p_rayVector.x;
    volumeIntersectionLeft.z = p_origin.z + parametricT * p_rayVector.z;
    if( this->Contains( volumeIntersectionLeft ) )
    {
        intersectionPoints.push_back( volumeIntersectionLeft );
        intersectionPointVoxels.push_back( this->FindVoxelContainingPosition( volumeIntersectionLeft ) );
        intersectionPointSides.push_back( Side::Left );
    }

    // Right check
    volumeIntersectionRight.y = m_pave.topRightBack.y;
    parametricT = ( volumeIntersectionRight.y - p_origin.y ) / p_rayVector.y;
    volumeIntersectionRight.x = p_origin.x + parametricT * p_rayVector.x;
    volumeIntersectionRight.z = p_origin.z + parametricT * p_rayVector.z;
    if( this->Contains( volumeIntersectionRight ) )
    {
        volumeIntersectionRight.y -= m_voxelSpacing.y;
        intersectionPointVoxels.push_back( this->FindVoxelContainingPosition( volumeIntersectionRight ) );
        volumeIntersectionRight.y += m_voxelSpacing.y;
        intersectionPoints.push_back( volumeIntersectionRight );
        intersectionPointSides.push_back( Side::Right );
    }

    // Front check
    volumeIntersectionFront.x = m_pave.bottomLeftFront.x;
    parametricT = ( volumeIntersectionFront.x - p_origin.x ) / p_rayVector.x;
    volumeIntersectionFront.y = p_origin.y + parametricT * p_rayVector.y;
    volumeIntersectionFront.z = p_origin.z + parametricT * p_rayVector.z;
    if( this->Contains( volumeIntersectionFront ) )
    {
        intersectionPoints.push_back( volumeIntersectionFront );
        intersectionPointVoxels.push_back( this->FindVoxelContainingPosition( volumeIntersectionFront ) );
        intersectionPointSides.push_back( Side::Front );
    }

    // Back check
    volumeIntersectionBack.x = m_pave.topRightBack.x;
    parametricT = ( volumeIntersectionBack.x - p_origin.x ) / p_rayVector.x;
    volumeIntersectionBack.y = p_origin.y + parametricT * p_rayVector.y;
    volumeIntersectionBack.z = p_origin.z + parametricT * p_rayVector.z;
    if( this->Contains( volumeIntersectionBack ) )
    {
        volumeIntersectionBack.x -= m_voxelSpacing.x;
        intersectionPointVoxels.push_back( this->FindVoxelContainingPosition( volumeIntersectionBack ) );
        volumeIntersectionBack.x += m_voxelSpacing.x;
        intersectionPoints.push_back( volumeIntersectionBack );
        intersectionPointSides.push_back( Side::Back );
    }

    if( intersectionPoints.empty() )
    {
        return std::vector<Position3D>();
    }
    if( intersectionPoints.size() != 2 )
    {
        qDebug() << "intersectionPointsWithVolumeFrontiers.size() != 2 but not empty : " << intersectionPoints.size();
        return std::vector<Position3D>();
    }

    Position3D bottomIntersectionPoint;
    Position3D topIntersectionPoint;
    Voxel bottomIntersectionVoxel;
    Voxel topIntersectionVoxel;
    if( intersectionPoints.front().z < intersectionPoints.back().z )
    {
        bottomIntersectionPoint = intersectionPoints.front();
        bottomIntersectionVoxel = intersectionPointVoxels.front();
        topIntersectionPoint = intersectionPoints.back();
        topIntersectionVoxel = intersectionPointVoxels.back();
    }
    else
    {
        bottomIntersectionPoint = intersectionPoints.back();
        bottomIntersectionVoxel = intersectionPointVoxels.back();
        topIntersectionPoint = intersectionPoints.front();
        topIntersectionVoxel = intersectionPointVoxels.front();
    }

    const auto & BeginIterationVoxel = topIntersectionVoxel;
    const auto & EndIterationVoxel = bottomIntersectionVoxel;


    // Z Way (no K Way ;))
    if( !AlmostEqualRelative( p_rayVector.z, 0.f ) && BeginIterationVoxel.z != EndIterationVoxel.z )
    {
        // no need to check the orientation
        for( auto voxelZ{ EndIterationVoxel.z + 1 }; voxelZ <= BeginIterationVoxel.z; ++voxelZ )
        {
            Position3D intersection;
            intersection.z = m_zPositions.at( voxelZ );

            auto parametricT = ( intersection.z - p_origin.z ) / p_rayVector.z;
            intersection.x = p_origin.x + parametricT * p_rayVector.x;
            intersection.y = p_origin.y + parametricT * p_rayVector.y;
            if( this->Contains( intersection ) )
            {
                intersectionPoints.push_back( intersection );
            }
        }
    }

    // Y Way (no K Way ;))
    if( !AlmostEqualRelative( p_rayVector.y, 0.f ) && BeginIterationVoxel.y != EndIterationVoxel.y )
    {
        auto minY{ BeginIterationVoxel.y + 1 };
        auto maxY{ EndIterationVoxel.y };
        if( p_rayVector.y < 0 )    // invert direction
        {
            minY = EndIterationVoxel.y + 1;
            maxY = BeginIterationVoxel.y;
        }
        for( auto voxelY{ minY }; voxelY <= maxY; ++voxelY )
        {
            Position3D intersection;
            intersection.y = m_yPositions.at( voxelY );

            auto parametricT = ( intersection.y - p_origin.y ) / p_rayVector.y;
            intersection.x = p_origin.x + parametricT * p_rayVector.x;
            intersection.z = p_origin.z + parametricT * p_rayVector.z;
            if( this->Contains( intersection ) )
            {
                intersectionPoints.push_back( intersection );
            }
        }
    }
    // X Way (no K Way ;))
    if( !AlmostEqualRelative( p_rayVector.x, 0.f ) && BeginIterationVoxel.x != EndIterationVoxel.x )
    {
        auto minX{ BeginIterationVoxel.x + 1 };
        auto maxX{ EndIterationVoxel.x };
        if( p_rayVector.x < 0 )    // invert direction
        {
            minX = EndIterationVoxel.x + 1;
            maxX = BeginIterationVoxel.x;
        }
        for( auto voxelX{ minX }; voxelX <= maxX; ++voxelX )
        {
            Position3D intersection;
            intersection.x = m_xPositions.at( voxelX );

            auto parametricT = ( intersection.x - p_origin.x ) / p_rayVector.x;
            intersection.y = p_origin.y + parametricT * p_rayVector.y;
            intersection.z = p_origin.z + parametricT * p_rayVector.z;
            if( this->Contains( intersection ) )
            {
                intersectionPoints.push_back( intersection );
            }
        }
    }

    return intersectionPoints;
}

Voxel TomoVolume::FindVoxelContainingPosition( const Position3D & p_position ) const
{
    Voxel voxel;
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
Position3D TomoVolume::GetBLF() const
{
    return m_pave.bottomLeftFront;
}
Position3D TomoVolume::GetTRB() const
{
    return m_pave.topRightBack;
}
VoxelSpacing TomoVolume::GetVoxelSpacing() const
{
    return m_voxelSpacing;
}

// for computation optimizations
const std::vector<float> * TomoVolume::GetXPositionsPtr() const
{
    return &m_xPositions;
}
const std::vector<float> * TomoVolume::GetYPositionsPtr() const
{
    return &m_yPositions;
}
const std::vector<float> * TomoVolume::GetZPositionsPtr() const
{
    return &m_zPositions;
}

const std::vector<Position3D> * TomoVolume::GetGridPositionsPtr() const
{
    return &m_gridPositions;
}
const std::vector<int> * TomoVolume::GetVoxelsIndicesPtr() const
{
    return &m_voxelsIndices;
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
    m_pave.wsize = ComputeWSize3D( m_size, m_voxelSpacing );
    m_pave.topRightBack = m_pave.bottomLeftFront + m_pave.wsize;
}
void TomoVolume::UpdateSize()
{
    m_size = ComputeSize3D( m_pave.wsize, m_voxelSpacing );
}
void TomoVolume::UpdateVoxelSpacing()
{
    m_voxelSpacing = ComputeVoxelSpacing( m_pave.wsize, m_size );
}

inline bool operator==( TomoVolume const & p_volumeA, TomoVolume const & p_volumeB )
{
    bool out = true;
    out &= p_volumeA.GetWSize3D() == p_volumeB.GetWSize3D();
    out &= p_volumeA.GetBLF() == p_volumeB.GetBLF();
    out &= p_volumeA.GetSize() == p_volumeB.GetSize();
    return out;
}

inline bool operator!=( TomoVolume const & p_volumeA, TomoVolume const & p_volumeB )
{
    return !( p_volumeA == p_volumeB );
}