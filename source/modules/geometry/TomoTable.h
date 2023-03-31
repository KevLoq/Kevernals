#pragma once

#include "modules/geometry/Position3D.h"

#include <vector>

class TomoTable
{
public:
    TomoTable() = default;
    ~TomoTable() = default;
    TomoTable( const std::vector<Position3D> & p_sourcesPositions, const float & p_detectorsZCommonPosition );

    friend std::ostream & operator<<( std::ostream & p_outputStream, TomoTable const & p_data );

    int GetSourcesNumber() const { return static_cast<int>( m_sourcesYPositions.size() ); }

    // sources geometric features
    std::vector<float> GetSourcesYPositions() const;
    float GetSourcesXCommonPosition() const;
    float GetSourcesZCommonPosition() const;

    float GetDetectorsZCommonPosition() const;

    void RemoveSource( std::vector<int> const & p_dataIndicesToRemove );

private:
    // sources geometric features
    std::vector<float> m_sourcesYPositions;
    float m_sourcesXCommonPosition{ 0.F };
    float m_sourcesZCommonPosition{ 0.F };

    float m_detectorsZCommonPosition{ 0.F };
};

inline std::ostream & operator<<( std::ostream & p_outputStream, TomoTable const & p_data )
{
    return p_outputStream << "sources X common position: " << p_data.m_sourcesXCommonPosition << std::endl
                          << "sources Z common position: " << p_data.m_sourcesZCommonPosition << std::endl
                          << "sources Y positions: " << p_data.m_sourcesYPositions << std::endl
                          << "detectors Z common position: " << p_data.m_detectorsZCommonPosition << std::endl;
}