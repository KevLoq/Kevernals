#pragma once

#include "modules/geometry/Dim3.h"

struct Size3D;
struct WSize3D;
struct VoxelSpacing : public Dim3<float>
{
    VoxelSpacing(float p_x,float p_y,float p_z): Dim3<float>(p_x,p_y,p_z) {} ;
    ~VoxelSpacing() override = default;
    static VoxelSpacing FromSizes( const Size3D & p_dimensions, const WSize3D & p_sizeInWorld, bool & p_allDimensionsValid );
};
