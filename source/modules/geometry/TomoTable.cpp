#include "modules/geometry/TomoTable.h"


TomoTable::TomoTable( const std::vector<Position3D> & p_sourcesPositions, const float & p_detectorsZCommonPosition )
  : m_detectorsZCommonPosition( p_detectorsZCommonPosition )
{
    auto sourcesSize = p_sourcesPositions.size();
    m_sourcesYPositions.resize( sourcesSize );
    m_sourcesXCommonPosition = 0.f;
    m_sourcesZCommonPosition = 0.f;
    for( size_t sourceIndex{ 0 }; sourceIndex < sourcesSize; sourceIndex++ )
    {
        m_sourcesXCommonPosition += p_sourcesPositions[sourceIndex].x;
        m_sourcesYPositions[sourceIndex] = p_sourcesPositions[sourceIndex].y;
        m_sourcesZCommonPosition += p_sourcesPositions[sourceIndex].z;
    }
    m_sourcesXCommonPosition /= static_cast<float>( sourcesSize );
    m_sourcesZCommonPosition /= static_cast<float>( sourcesSize );
}

// sources geometric features
std::vector<float> TomoTable::GetSourcesYPositions() const
{
    return m_sourcesYPositions;
}
float TomoTable::GetSourcesXCommonPosition() const
{
    return m_sourcesXCommonPosition;
}
float TomoTable::GetSourcesZCommonPosition() const
{
    return m_sourcesZCommonPosition;
}

float TomoTable::GetDetectorsZCommonPosition() const
{
    return m_detectorsZCommonPosition;
}
