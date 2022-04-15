#pragma once

#include "commons/GeometryUtils.h"

#include <vector>

class TomoTable
{
public:
    TomoTable() = default;
    ~TomoTable() = default;
    TomoTable( const Position3D & p_rotationCenter, const std::vector<Position3D> & p_sourcesPositions, const float & p_detectorsZCommonPosition );

    int GetSourcesNumber() const { return static_cast<int>( m_sourcesYPositions.size() ); }

    Position3D GetRotationCenter() const;

    // sources geometric features
    const std::vector<float> * GetSourcesYPositionsPtr() const;
    float GetSourcesXCommonPosition() const;
    float GetSourcesZCommonPosition() const;

    float GetDetectorsZCommonPosition() const;

private:
    Position3D m_rotationCenter;

    // sources geometric features
    std::vector<float> m_sourcesYPositions;
    float m_sourcesXCommonPosition{ 0.f };
    float m_sourcesZCommonPosition{ 0.f };

    float m_detectorsZCommonPosition{ 0.f };
};
