#pragma once

#include "modules/geometry/Dim3.h"

struct VoxelSpacing;
struct Size3D;
struct WSize3D : public Dim3<float>
{
    WSize3D( float p_x, float p_y, float p_z )
      : Dim3<float>( p_x, p_y, p_z ){};
    ~WSize3D() override = default;
    WSize3D( const VoxelSpacing & p_voxelSpacing, const Size3D & p_size );
    WSize3D & operator=( Dim3<float> const & p_other )
    {
        x = p_other.x;
        y = p_other.y;
        z = p_other.z;
        return *this;
    }
};