#pragma once

#include "modules/toys/ToysGeometryUtils.h"

#include <memory>
#include <vector>

class ToySet
{
public:
    ToySet() = default;
    ~ToySet() = default;

    ToySet( const std::vector<Position3D> & p_positions );

    std::vector<Position3D> GetPositionsCopy() const;
    std::vector<Position3D> GetPositionsRValue();
    const std::vector<Position3D> * GetPositionsPtr() const;

private:
    std::vector<Position3D> m_positions;
};
