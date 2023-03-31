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

void TomoTable::RemoveSource( std::vector<int> const & p_dataIndicesToRemove )
{
    auto indices = p_dataIndicesToRemove;
    std::sort( std::begin( indices ), std::end( indices ), std::greater<int>() );

    std::cout << "RemoveSource" << std::endl;
    for( auto index : indices )
    {
        std::cout << index << std::endl;
        if( index < 0 || index >= static_cast<int>( m_sourcesYPositions.size() ) )
        {
            continue;
        }
        m_sourcesYPositions.erase( std::begin( m_sourcesYPositions ) + index );
    }
}