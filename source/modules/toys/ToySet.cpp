#pragma once

#include "modules/toys/ToySet.h"


ToySet::ToySet( const std::vector<Position3D> & p_positions )
  : m_positions( p_positions )
{}


std::vector<Position3D> ToySet::GetPositionsCopy() const
{
    return m_positions;
}

std::vector<Position3D> ToySet::GetPositionsRValue()
{
    return std::move( m_positions );
}

const std::vector<Position3D> * ToySet::GetPositionsPtr() const
{
    return &m_positions;
}